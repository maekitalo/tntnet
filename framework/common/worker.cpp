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
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <cxxtools/dlloader.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/net/tcpserver.h>
#include <pthread.h>
#include <string.h>
#include "config.h"

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
  cxxtools::Mutex Worker::_mutex;
  Worker::workers_type Worker::_workers;
  Comploader Worker::_comploader;

  Worker::Worker(Tntnet& app)
    : _application(app),
      _threadId(0),
      _state(stateStarting),
      _lastWaitTime(0)
  {
    cxxtools::MutexLock lock(_mutex);
    _workers.insert(this);
  }

  void Worker::run()
  {
    _threadId = pthread_self();
    Jobqueue& queue = _application.getQueue();
    log_debug("start thread " << _threadId);
    while (queue.getWaitThreadCount() < _application.getMinThreads())
    {
      _state = stateWaitingForJob;
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
          time(&_lastWaitTime);

          keepAlive = false;
          _state = stateParsing;
          try
          {
            j->getParser().parse(socket);
            _state = statePostParsing;

            if (socket.eof())
              log_debug("eof");
            else if (j->getParser().failed())
            {
              _state = stateSendError;
              log_warn("bad request");
              tnt::HttpReply errorReply(socket);
              errorReply.setVersion(1, 0);
              errorReply.setContentType("text/html");
              errorReply.setKeepAliveCounter(0);
              errorReply.out() << "<html><body><h1>Error</h1><p>bad request</p></body></html>\n";
              errorReply.sendReply(400, "Bad Request");
              logRequest(j->getRequest(), errorReply, 400);
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
                    if (::poll(&fd, 1, TntConfig::it().socketReadTimeout) == 0)
                    {
                      log_debug("pass job to poll-thread");
                      _application.getPoller().addIdleJob(j);
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
            _state = stateSendError;
            log_warn("http-Error: " << e.what());
            HttpReply errorReply(socket);
            errorReply.setVersion(1, 0);
            errorReply.setKeepAliveCounter(0);
            for (HttpMessage::header_type::const_iterator it = e.header_begin();
                 it != e.header_end(); ++it)
              errorReply.setHeader(it->first, it->second);

            errorReply.out() << e.getBody() << '\n';
            errorReply.sendReply(e.getErrcode(), e.getErrmsg());
            logRequest(j->getRequest(), errorReply, e.getErrcode());
          }
        } while (keepAlive);
      }
      catch (const cxxtools::IOTimeout& e)
      {
        _application.getPoller().addIdleJob(j);
      }
      catch (const cxxtools::net::AcceptTerminated&)
      {
        log_debug("listener terminated");
        break;
      }
      catch (const std::exception& e)
      {
        log_warn("unexpected exception: " << e.what());
      }
    }

    time(&_lastWaitTime);

    _state = stateStopping;

    cxxtools::MutexLock lock(_mutex);
    _workers.erase(this);

    log_debug("end worker thread " << _threadId << " - " << _workers.size()
      << " threads left - " << _application.getQueue().getWaitThreadCount()
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

#ifdef ENABLE_LOCALE
    reply.setLocale(request.getLocale());
#endif

    if (request.keepAlive())
      reply.setKeepAliveCounter(keepAliveCount);

    if (TntConfig::it().enableCompression)
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
      _state = stateSendError;
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
      logRequest(request, errorReply, e.getErrcode());
    }

    return keepAliveCount > 0;
  }

  void Worker::logRequest(const HttpRequest& request, const HttpReply& reply, unsigned httpReturn)
  {
    static cxxtools::atomic_t waitCount = 0;
    cxxtools::atomicIncrement(waitCount);

    const std::string& fname = TntConfig::it().accessLog;
    if (fname.empty())
      return;

    std::ofstream& accessLog = _application._accessLog;

    if (!accessLog.is_open())
    {
      cxxtools::MutexLock lock(_application._accessLogMutex);

      if (!accessLog.is_open())
      {
        log_debug("access log is not open - open now");
        accessLog.open(fname.c_str(), std::ios::out | std::ios::app);
        if (accessLog.fail())
        {
          std::cerr << "failed to open access log \"" << fname << '"' << std::endl;
          TntConfig::it().accessLog.clear();
        }
      }
    }

    log_debug("log request to access log with return code " << httpReturn);

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

    time_t t;
    ::time(&t);

    cxxtools::MutexLock lock(_application._accessLogMutex);

    // cache for timestamp of access log
    static time_t lastLogTime = 0;
    static char timebuf[40];

    if (t != lastLogTime)
    {
      struct tm tm;
      ::localtime_r(&t, &tm);
      strftime(timebuf, sizeof(timebuf), "%d/%b/%Y:%H:%M:%S %z", &tm);
      lastLogTime = t;
    }

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
              << request.getHeader(httpheader::userAgent, "-") << "\"\n";
    if (cxxtools::atomicDecrement(waitCount) == 0)
      accessLog.flush();
  }

  void Worker::dispatch(HttpRequest& request, HttpReply& reply)
  {
    _state = stateDispatch;
    const std::string& url = request.getUrl();

    if (!HttpRequest::checkUrl(url))
    {
      log_info("illegal url <" << url << '>');
      throw HttpError(HTTP_BAD_REQUEST, "illegal url");
    }

    request.setThreadContext(this);

    Dispatcher::PosType pos(_application.getDispatcher(), request);
    while (true)
    {
      _state = stateDispatch;

      // pos.getNext() throws NotFoundException at end
      Maptarget ci = pos.getNext();
      try
      {
        Component* comp = 0;
        try
        {
          if (ci.libname == _application.getAppName())
          {
            // if the libname is the app name look first, if the component is
            // linked directly
            try
            {
              Compident cii = ci;
              cii.libname = std::string();
              comp = &_comploader.fetchComp(cii, _application.getDispatcher());
            }
            catch (const NotFoundException&)
            {
              // if the component is not found in the binary, fetchComp throws
              // NotFoundException and comp remains 0.
              // so we can ignore the exceptioni and just continue
            }
          }

          if (comp == 0)
            comp = &_comploader.fetchComp(ci, _application.getDispatcher());
        }
        catch (const NotFoundException& e)
        {
          log_debug("NotFoundException catched - url " << e.getUrl() << " try next mapping");
          continue;
        }

        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());

        std::string appname = _application.getAppName().empty() ? ci.libname : _application.getAppName();

        _application.getScopemanager().preCall(request, appname);

        _state = stateProcessingRequest;
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
            _state = stateFlush;
            reply.out().flush();
          }
          else
          {
            log_info("request " << request.getMethod_cstr() << ' ' << request.getQuery() << " ready, returncode " << http_return << ' ' << http_msg << " - ContentSize: " << reply.getContentSize());

            _application.getScopemanager().postCall(request, reply, appname);

            _state = stateSendReply;
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

    cxxtools::MutexLock lock(_mutex);
    for (workers_type::iterator it = _workers.begin(); it != _workers.end(); ++it)
    {
      (*it)->healthCheck(currentTime);
    }
  }

  void Worker::healthCheck(time_t currentTime)
  {
    if (_state == stateProcessingRequest
        && _lastWaitTime != 0
        && TntConfig::it().maxRequestTime > 0)
    {
      if (static_cast<unsigned>(currentTime - _lastWaitTime) > TntConfig::it().maxRequestTime)
      {
        log_fatal("requesttime " << TntConfig::it().maxRequestTime << " seconds in thread "
          << _threadId << " exceeded - exit process");
        log_info("current state: " << _state);
        ::_exit(111);
      }
    }
  }

  void Worker::touch()
    { time(&_lastWaitTime); }

  Scope& Worker::getScope()
    { return _threadScope; }

  Worker::workers_type::size_type Worker::getCountThreads()
  {
    cxxtools::MutexLock lock(_mutex);
    return _workers.size();
  }
}

