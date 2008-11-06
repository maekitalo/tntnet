/* worker.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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
#include <locale>
#include <errno.h>
#include <sys/poll.h>

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
      lastWaitTime(0)
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
      log_debug("waiting for job");
      state = stateWaitingForJob;
      Jobqueue::JobPtr j = queue.get();
      if (Tntnet::shouldStop())
      {
        // put job back to queue to wake up next worker if any left
        queue.put(j);
        break;
      }

      log_debug("got job - fd=" << j->getFd());

      try
      {
        std::iostream& socket = j->getStream();
        if (Tntnet::shouldStop())
          break;

        bool keepAlive;
        do
        {
          time(&lastWaitTime);

          log_debug("read request");

          keepAlive = false;
          state = stateParsing;
          j->getParser().parse(socket);
          state = statePostParsing;

          if (socket.eof())
            log_debug("eof");
          else if (j->getParser().failed())
          {
            state = stateSendError;
            log_warn("bad request");
            socket << "HTTP/1.0 500 bad request\r\n"
                      "Content-Type: text/html\r\n"
                      "\r\n"
                      "<html><body><h1>Error</h1><p>bad request</p></body></html>"
                   << std::endl;
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

              // if there is something to do and no threads waiting, we take
              // the next job just to improve resposiveness.
              if (queue.getWaitThreadCount() == 0
                && !queue.empty())
              {
                log_debug("put job back into queue");
                queue.put(j, true);
                keepAlive = false;
              }
              else
              {
                struct pollfd fd;
                fd.fd = j->getFd();
                fd.events = POLLIN;
                log_debug("wait for next request (timeout " << Job::getSocketReadTimeout() << ')');
                if (::poll(&fd, 1, Job::getSocketReadTimeout()) == 0)
                {
                  log_debug("pass job to poll-thread");
                  application.getPoller().addIdleJob(j);
                  keepAlive = false;
                }
              }
            }
          }
        } while (keepAlive);
      }
      catch (const cxxtools::net::Timeout& e)
      {
        log_debug("timeout - put job in poller");
        application.getPoller().addIdleJob(j);
      }
      catch (const cxxtools::net::Exception& e)
      {
        if (e.getErrno() != ENOENT)
          log_warn("unexpected exception: " << e.what());
      }
      catch (const std::exception& e)
      {
        log_warn("unexpected exception: " << e.what());
      }
    }

    time(&lastWaitTime);

    log_info("end worker thread " << threadId);

    state = stateStopping;

    cxxtools::MutexLock lock(mutex);
    workers.erase(this);

    log_debug("delete worker " << threadId << " - " << workers.size()
      << " threads left - " << application.getQueue().getWaitThreadCount()
      << " waiting threads");
  }

  bool Worker::processRequest(HttpRequest& request, std::iostream& socket,
         unsigned keepAliveCount)
  {
    // log message
    log_info("request " << request.getMethod() << ' ' << request.getQuery()
      << " from client " << request.getPeerIp() << " user-Agent \"" << request.getUserAgent()
      << '"');

    // create reply-object
    HttpReply reply(socket);
    reply.setVersion(request.getMajorVersion(), request.getMinorVersion());
    if (request.getMethod() == "HEAD")
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
      HttpReply reply(socket);
      reply.setVersion(request.getMajorVersion(), request.getMinorVersion());
      if (request.keepAlive())
        reply.setKeepAliveCounter(keepAliveCount);
      else
        keepAliveCount = 0;
      for (HttpMessage::header_type::const_iterator it = e.header_begin();
           it != e.header_end(); ++it)
        reply.setHeader(it->first, it->second);

      reply.out() << e.getBody();
      reply.sendReply(e.getErrcode(), e.getErrmsg());
    }

    return keepAliveCount > 0;
  }

  void Worker::dispatch(HttpRequest& request, HttpReply& reply)
  {
    state = stateDispatch;
    const std::string& url = request.getUrl();

    log_debug("dispatch " << request.getQuery());

    if (!HttpRequest::checkUrl(url))
      throw HttpError(HTTP_BAD_REQUEST, "illegal url");

    request.setThreadScope(threadScope);

    Dispatcher::PosType pos(application.getDispatcher(), request.getHost(),
      request.getUrl());
    while (true)
    {
      state = stateDispatch;

      // pos.getNext() throws NotFoundException at end
      Dispatcher::CompidentType ci = pos.getNext();
      try
      {
        log_debug("load component " << ci);
        Component* comp;
        try
        {
          comp = &comploader.fetchComp(ci, application.getDispatcher());
        }
        catch (const NotFoundException& e)
        {
          log_debug("NotFoundException catched - url " << e.getUrl() << " try next mapping");
          continue;
        }

        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());

        application.getScopemanager().preCall(request, ci.libname);

        log_debug("call component " << ci << " path " << request.getPathInfo());
        state = stateProcessingRequest;
        unsigned http_return = (*comp)(request, reply, request.getQueryParams(), true);
        if (http_return != DECLINED)
        {
          if (reply.isDirectMode())
          {
            log_info("request " << request.getMethod() << ' ' << request.getQuery() << " ready, returncode " << http_return);
            state = stateFlush;
            reply.out().flush();
          }
          else
          {
            log_info("request " << request.getMethod() << ' ' << request.getQuery() << " ready, returncode " << http_return << " - ContentSize: " << reply.getContentSize());

            application.getScopemanager().postCall(request, reply, ci.libname);

            state = stateSendReply;
            reply.sendReply(http_return);
          }

          if (reply.out())
            log_debug("reply sent");
          else
          {
            reply.setKeepAliveCounter(0);
            log_warn("stream error");
          }

          return;
        }
        else
          log_debug("component " << ci << " returned DECLINED");
      }
      catch (const cxxtools::dl::DlopenError& e)
      {
        log_warn("DlopenError catched - libname " << e.getLibname());
      }
      catch (const cxxtools::dl::SymbolNotFound& e)
      {
        log_warn("SymbolNotFound catched - symbol " << e.getSymbol());
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

  Worker::workers_type::size_type Worker::getCountThreads()
  {
    cxxtools::MutexLock lock(mutex);
    return workers.size();
  }
}
