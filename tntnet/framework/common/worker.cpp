/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "tnt/worker.h"
#include "tnt/dispatcher.h"
#include "tnt/job.h"
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httperror.h>
#include <tnt/http.h>
#include <tnt/poller.h>
#include <cxxtools/log.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <cxxtools/dlloader.h>
#include <cxxtools/ioerror.h>
#include <pthread.h>
#include <string.h>

log_define("tntnet.worker")

namespace
{
  static const char stateStarting[]          = "0 starting";
  static const char stateWaitingForJob[]     = "1 waiting for job";
  static const char stateParsing[]           = "2 parsing request";
  static const char statePostParsing[]       = "3 post parsing";
  static const char stateDispatch[]          = "4 dispatch";
  static const char stateProcessingRequest[] = "5 processing request";
  static const char stateFlush[]             = "6 flush";
  static const char stateSendReply[]         = "7 send reply";
  static const char stateSendError[]         = "8 send error";
  static const char stateStopping[]          = "9 stopping";
}

namespace tnt
{
  cxxtools::Mutex Worker::mutex;
  Worker::workers_type Worker::workers;
  unsigned Worker::maxRequestTime = 600;
  bool Worker::enableCompression = true;
  Comploader Worker::comploader;

  Worker::Worker(Tntnet& app)
    : application(app),
      threadId(0),
      state(stateStarting),
      lastWaitTime(0),
      lastLogTime(0)
  {
    cxxtools::MutexLock lock(mutex);
    workers.insert(this);
  }

  void Worker::run()
  {
    threadId = pthread_self();
    Jobqueue& queue = application.getQueue();
    log_debug("start thread " << threadId);
    while (queue.getWaitThreadCount() < application.getMinThreads())
    {
      state = stateWaitingForJob;
      Jobqueue::JobPtr j = queue.get();
      if (Tntnet::shouldStop())
      {
        // put job back to queue to wake up next worker if any left
        queue.put(j);
        break;
      }

      try
      {
        std::iostream& socket = j->getStream();
        if (Tntnet::shouldStop())
          break;

        bool keepAlive;
        do
        {
          time(&lastWaitTime);

          keepAlive = false;
          state = stateParsing;
          try
          {
            j->getParser().parse(socket);
            state = statePostParsing;

            if (socket.eof())
              log_debug("eof");
            else if (j->getParser().failed())
            {
              state = stateSendError;
              log_warn("bad request");
              tnt::HttpReply errorReply(socket);
              errorReply.setVersion(1, 0);
              errorReply.setContentType("text/html");
              errorReply.setKeepAliveCounter(0);
              errorReply.out() << "<html><body><h1>Error</h1><p>bad request</p></body></html>\n";
              errorReply.sendReply(400, "Bad Request");
              HttpRequest request(application);
              request.setMethod("-");
              logRequest(request, errorReply, 400);
            }
            else if (socket.fail())
              log_debug("socket failed");
            else
            {
              j->getRequest().doPostParse();

              j->setWrite();
              keepAlive = processRequest(j->getRequest(), socket,
                j->decrementKeepAliveCounter());

              if (keepAlive)
              {
                j->setRead();
                j->clear();

                if (!socket.rdbuf()->in_avail())
                {
                  if (queue.getWaitThreadCount() == 0
                    && !queue.empty())
                  {
                    // if there is something to do and no threads waiting, we take
                    // the next job just to improve responsiveness.
                    log_debug("put job back into queue");
                    queue.put(j, true);
                    keepAlive = false;
                  }
                  else
                  {
                    struct pollfd fd;
                    fd.fd = j->getFd();
                    fd.events = POLLIN;
                    if (::poll(&fd, 1, Job::getSocketReadTimeout()) == 0)
                    {
                      log_debug("pass job to poll-thread");
                      application.getPoller().addIdleJob(j);
                      keepAlive = false;
                    }
                  }
                }
              }
            }
          }
          catch (const HttpError& e)
          {
            keepAlive = false;
            state = stateSendError;
            log_warn("http-Error: " << e.what());
            HttpReply errorReply(socket);
            errorReply.setVersion(1, 0);
            errorReply.setKeepAliveCounter(0);
            for (HttpMessage::header_type::const_iterator it = e.header_begin();
                 it != e.header_end(); ++it)
              errorReply.setHeader(it->first, it->second);

            errorReply.out() << e.getBody() << '\n';
            errorReply.sendReply(e.getErrcode(), e.getErrmsg());
            HttpRequest request(application);
            request.setMethod("-");
            logRequest(request, errorReply, e.getErrcode());
          }
        } while (keepAlive);
      }
      catch (const cxxtools::IOTimeout& e)
      {
        application.getPoller().addIdleJob(j);
      }
      catch (const std::exception& e)
      {
        log_warn("unexpected exception: " << e.what());
      }
    }

    time(&lastWaitTime);

    state = stateStopping;

    cxxtools::MutexLock lock(mutex);
    workers.erase(this);

    log_debug("end worker thread " << threadId << " - " << workers.size()
      << " threads left - " << application.getQueue().getWaitThreadCount()
      << " waiting threads");
  }

  bool Worker::processRequest(HttpRequest& request, std::iostream& socket,
         unsigned keepAliveCount)
  {
    // log message
    log_info("request " << request.getMethod_cstr() << ' ' << request.getQuery()
      << " from client " << request.getPeerIp() << " user-Agent \"" << request.getUserAgent()
      << "\" user \"" << request.getUsername() << '"');

    // create reply-object
    HttpReply reply(socket);
    reply.setVersion(request.getMajorVersion(), request.getMinorVersion());
    if (request.isMethodHEAD())
      reply.setHeadRequest();

    reply.setLocale(request.getLocale());

    if (request.keepAlive())
      reply.setKeepAliveCounter(keepAliveCount);

    if (enableCompression)
      reply.setAcceptEncoding(request.getEncoding());

    // process request
    try
    {
      try
      {
        dispatch(request, reply);

        if (!request.keepAlive() || !reply.keepAlive())
          keepAliveCount = 0;

        if (keepAliveCount > 0)
          log_debug("keep alive");
        else
        {
          log_debug("no keep alive request/reply="
              << request.keepAlive() << '/' << reply.keepAlive());
        }
      }
      catch (const HttpError& e)
      {
        throw;
      }
      catch (const std::exception& e)
      {
        throw HttpError(HTTP_INTERNAL_SERVER_ERROR, e.what());
      }
      catch (...)
      {
        log_error("unknown exception");
        throw HttpError(HTTP_INTERNAL_SERVER_ERROR, "unknown error");
      }
    }
    catch (const HttpError& e)
    {
      state = stateSendError;
      log_warn("http-Error: " << e.what());
      HttpReply errorReply(socket);
      errorReply.setVersion(request.getMajorVersion(), request.getMinorVersion());
      if (request.keepAlive())
        errorReply.setKeepAliveCounter(keepAliveCount);
      else
        keepAliveCount = 0;
      for (HttpMessage::header_type::const_iterator it = e.header_begin();
           it != e.header_end(); ++it)
        errorReply.setHeader(it->first, it->second);

      errorReply.out() << e.getBody() << '\n';
      errorReply.sendReply(e.getErrcode(), e.getErrmsg());
      HttpRequest request(application);
      request.setMethod("-");
      logRequest(request, errorReply, e.getErrcode());
    }

    return keepAliveCount > 0;
  }

  void Worker::logRequest(const HttpRequest& request, const HttpReply& reply, unsigned httpReturn)
  {
    std::ofstream& accessLog = application.accessLog;

    if (!accessLog.is_open())
      return;

    log_debug("log requests with return code " << httpReturn);

    time_t t;
    ::time(&t);
    if (t != lastLogTime)
    {
      struct tm tm;
      ::localtime_r(&t, &tm);
      strftime(timebuf, sizeof(timebuf), "%d/%b/%Y:%H:%M:%S %z", &tm);
      lastLogTime = t;
    }

    cxxtools::MutexLock lock(application.accessLogMutex);

    static const std::string unknown("-");

    std::string user = request.getUsername();
    if (user.empty())
      user = unknown;

    std::string peerIp = request.getPeerIp();
    if (peerIp.empty())
      peerIp = unknown;

    std::string query = request.getQuery();
    if (query.empty())
      query = unknown;

    accessLog << peerIp
              << " - " << user << " [" << timebuf << "] \""
              << request.getMethod_cstr() << ' '
              << query << ' '
              << "HTTP/" << request.getMajorVersion() << '.' << request.getMinorVersion() << "\" "
              << httpReturn << ' ';
    std::string::size_type contentSize = reply.getContentSize();
    if (contentSize != 0)
      accessLog << contentSize;
    else
      accessLog << '-';
    accessLog << " \"" << request.getHeader(httpheader::referer, "-") << "\" \""
              << request.getHeader(httpheader::userAgent, "-") << "\""
              << std::endl;
  }

  void Worker::dispatch(HttpRequest& request, HttpReply& reply)
  {
    state = stateDispatch;
    const std::string& url = request.getUrl();

    if (!HttpRequest::checkUrl(url))
    {
      log_info("illegal url <" << url << '>');
      throw HttpError(HTTP_BAD_REQUEST, "illegal url");
    }

    request.setThreadContext(this);

    Dispatcher::PosType pos(application.getDispatcher(), request.getHost(),
      request.getUrl());
    while (true)
    {
      state = stateDispatch;

      // pos.getNext() throws NotFoundException at end
      Dispatcher::CompidentType ci = pos.getNext();
      try
      {
        Component* comp = 0;
        try
        {
          if (ci.libname == application.getAppName())
          {
            // if the libname is the app name look first, if the component is
            // linked directly
            try
            {
              Dispatcher::CompidentType cii = ci;
              cii.libname = std::string();
              comp = &comploader.fetchComp(cii, application.getDispatcher());
            }
            catch (const NotFoundException&)
            {
              // if the component is not found in the binary, fetchComp throws
              // NotFoundException and comp remains 0.
              // so we can ignore the exceptioni and just continue
            }
          }

          if (comp == 0)
            comp = &comploader.fetchComp(ci, application.getDispatcher());
        }
        catch (const NotFoundException& e)
        {
          log_debug("NotFoundException catched - url " << e.getUrl() << " try next mapping");
          continue;
        }

        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());

        std::string appname = application.getAppName().empty() ? ci.libname : application.getAppName();

        application.getScopemanager().preCall(request, appname);

        state = stateProcessingRequest;
        unsigned http_return;
        const char* http_msg;
        std::string msg;
        try
        {
          http_return = comp->topCall(request, reply, request.getQueryParams());
          http_msg = HttpReturn::httpMessage(http_return);
        }
        catch (const HttpReturn& e)
        {
          http_return = e.getReturnCode();
          msg = e.getMessage();
          http_msg = msg.c_str();
        }

        if (http_return != DECLINED)
        {
          if (reply.isDirectMode())
          {
            log_info("request " << request.getMethod_cstr() << ' ' << request.getQuery() << " ready, returncode " << http_return << ' ' << http_msg);
            state = stateFlush;
            reply.out().flush();
          }
          else
          {
            log_info("request " << request.getMethod_cstr() << ' ' << request.getQuery() << " ready, returncode " << http_return << ' ' << http_msg << " - ContentSize: " << reply.getContentSize());

            application.getScopemanager().postCall(request, reply, appname);

            state = stateSendReply;
            reply.sendReply(http_return, http_msg);
          }

          logRequest(request, reply, http_return);

          if (reply.out())
            log_debug("reply sent");
          else
          {
            reply.setKeepAliveCounter(0);
            log_warn("sending failed");
          }

          return;
        }
        else
          log_debug("component " << ci << " returned DECLINED");
      }
      catch (const LibraryNotFound& e)
      {
        log_warn("library " << e.getLibname() << " not found");
      }
    }

    throw NotFoundException(request.getUrl());
  }

  void Worker::timer()
  {
    time_t currentTime;
    time(&currentTime);

    cxxtools::MutexLock lock(mutex);
    for (workers_type::iterator it = workers.begin();
         it != workers.end(); ++it)
    {
      (*it)->healthCheck(currentTime);
    }
  }

  void Worker::healthCheck(time_t currentTime)
  {
    if (state == stateProcessingRequest
        && lastWaitTime != 0
        && maxRequestTime > 0)
    {
      if (static_cast<unsigned>(currentTime - lastWaitTime) > maxRequestTime)
      {
        log_fatal("requesttime " << maxRequestTime << " seconds in thread "
          << threadId << " exceeded - exit process");
        log_info("current state: " << state);
        ::exit(111);
      }
    }
  }

  void Worker::touch()
  {
    time(&lastWaitTime);
  }

  Scope& Worker::getScope()
  {
    return threadScope;
  }

  Worker::workers_type::size_type Worker::getCountThreads()
  {
    cxxtools::MutexLock lock(mutex);
    return workers.size();
  }
}
