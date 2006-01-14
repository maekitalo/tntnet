/* worker.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "tnt/worker.h"
#include "tnt/dispatcher.h"
#include "tnt/job.h"
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/poller.h>
#include <tnt/sessionscope.h>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

log_define("tntnet.worker")

namespace
{
  class ComponentUnloadLock
  {
      tnt::Component& comp;

    public:
      ComponentUnloadLock(tnt::Component& c)
        : comp(c)
      { comp.lock(); }
      ~ComponentUnloadLock()
      { comp.unlock(); }
  };

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
  unsigned Worker::nextThreadNumber = 0;
  Worker::workers_type Worker::workers;
  unsigned Worker::compLifetime = 600;
  unsigned Worker::maxRequestTime = 600;
  unsigned Worker::reportStateTime = 0;
  time_t Worker::nextReportStateTime = 0;
  unsigned Worker::minThreads = 5;
  bool Worker::enableCompression = false;

  Worker::ComploaderPoolType Worker::comploaderPool;

  Worker::Worker(Tntnet& app)
    : application(app),
      comploaderObject(comploaderPool.get()),
      comploader(comploaderObject),
      threadId(0),
      state(stateStarting),
      lastWaitTime(0)
  {
    log_debug("initialize thread " << threadId);

    cxxtools::MutexLock lock(mutex);
    workers.insert(this);
  }

  Worker::~Worker()
  {
    cxxtools::MutexLock lock(mutex);
    workers.erase(this);
    comploader.cleanup(0);

    log_debug("delete worker " << threadId << " - " << workers.size() << " threads left - " << application.getQueue().getWaitThreadCount() << " waiting threads");
  }

  void Worker::run()
  {
    threadId = pthread_self();
    Jobqueue& queue = application.getQueue();
    log_debug("start thread " << threadId);
    while (queue.getWaitThreadCount() < minThreads)
    {
      log_debug("waiting for job");
      state = stateWaitingForJob;
      Jobqueue::JobPtr j = queue.get();
      log_debug("got job - fd=" << j->getFd());

      std::iostream& socket = j->getStream();

      try
      {
        bool keepAlive;
        do
        {
          time(&lastWaitTime);

          keepAlive = false;
          log_debug("call parser");
          state = stateParsing;
          j->getParser().parse(socket);
          state = statePostParsing;

          if (socket.eof())
            log_debug("eof");
          else if (j->getParser().failed())
            log_error("end parser");
          else if (socket.fail())
            log_error("socket failed");
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
                application.getPoller().addIdleJob(j);
                keepAlive = false;
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
      catch (const std::exception& e)
      {
        log_warn("unexpected exception: " << e.what());
      }
    }

    time(&lastWaitTime);

    log_debug("end worker-thread " << threadId);

    state = stateStopping;
  }

  bool Worker::processRequest(HttpRequest& request, std::iostream& socket,
         unsigned keepAliveCount)
  {
    // log message
    log_info("process request: " << request.getMethod() << ' ' << request.getUrl()
      << " from client " << request.getPeerIp());
    log_info("user-Agent \"" << request.getUserAgent() << '"');

    // create reply-object
    HttpReply reply(socket);
    reply.setVersion(request.getMajorVersion(), request.getMinorVersion());
    reply.setMethod(request.getMethod());

    if (request.keepAlive())
      reply.setKeepAliveCounter(keepAliveCount);

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
      //catch (const cxxtools::Net::Timeout& t)
      //{
          // TODO pass job to writer-thread
          // return false;
      //}
      catch (const std::exception& e)
      {
        throw HttpError(HTTP_INTERNAL_SERVER_ERROR, e.what());
      }
    }
    catch (const HttpError& e)
    {
      state = stateSendError;
      log_warn("http-Error: " << e.what());
      socket << "HTTP/1.0 " << e.what()
             << "\r\n\r\n"
             << "<html><body><h1>Error</h1><p>"
             << e.what() << "</p></body></html>" << std::endl;
      return false;
    }

    return keepAliveCount > 0;
  }

  void Worker::dispatch(HttpRequest& request, HttpReply& reply)
  {
    state = stateDispatch;
    const std::string& url = request.getUrl();

    log_info("dispatch " << request.getQuery());

    if (!HttpRequest::checkUrl(url))
      throw HttpError(HTTP_BAD_REQUEST, "illegal url");

    Dispatcher::PosType pos(application.getDispatcher(), request.getUrl());
    while (true)
    {
      state = stateDispatch;

      // pos.getNext() throws NotFoundException at end
      Dispatcher::CompidentType ci = pos.getNext();
      try
      {
        log_debug("load component " << ci);
        Component& comp = comploader.fetchComp(ci, application.getDispatcher());
        ComponentUnloadLock unload_lock(comp);
        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());

        application.getScopemanager().preCall(request, ci.libname);

        log_info("call component " << ci << " path " << request.getPathInfo());
        state = stateProcessingRequest;
        unsigned http_return = comp(request, reply, request.getQueryParams());
        if (http_return != DECLINED)
        {
          if (reply.isDirectMode())
          {
            log_info("request ready, returncode " << http_return);
            state = stateFlush;
            reply.out().flush();
          }
          else
          {
            log_info("request ready, returncode " << http_return << " - ContentSize: " << reply.getContentSize());

            application.getScopemanager().postCall(request, reply, ci.libname);

            if (enableCompression)
              reply.setAcceptEncoding(request.getEncoding());

            state = stateSendReply;
            reply.sendReply(http_return);
          }

          if (reply.out())
            log_info("reply sent");

          return;
        }
        else
          log_debug("component " << ci << " returned DECLINED");
      }
      catch (const cxxtools::dl::DlopenError& e)
      {
        log_warn("dl::DlopenError catched - libname " << e.getLibname());
      }
      catch (const cxxtools::dl::SymbolNotFound& e)
      {
        log_warn("dl::SymbolNotFound catched - symbol " << e.getSymbol());
      }
    }

    throw NotFoundException(request.getUrl());
  }

  void Worker::timer()
  {
    time_t currentTime;
    time(&currentTime);
    bool reportState = false;
    if (reportStateTime > 0 && currentTime > nextReportStateTime)
    {
      if (nextReportStateTime)
        reportState = true;
      nextReportStateTime = currentTime + reportStateTime;
    }

    cxxtools::MutexLock lock(mutex);
    for (workers_type::iterator it = workers.begin();
         it != workers.end(); ++it)
    {
      (*it)->healthCheck(currentTime);
      (*it)->cleanup(compLifetime);
      if (reportState)
        log_info("threadstate " << (*it)->threadId << ": " << (*it)->state);
    }
  }

  void Worker::healthCheck(time_t currentTime)
  {
    if (state != stateWaitingForJob
        && lastWaitTime != 0
        && maxRequestTime > 0)
    {
      if (currentTime - lastWaitTime > maxRequestTime)
      {
        log_fatal("requesttime " << maxRequestTime << " seconds in thread "
          << threadId << " exceeded - exit process");
        log_info("current state: " << state);
        exit(111);
      }
    }
  }

  Worker::workers_type::size_type Worker::getCountThreads()
  {
    cxxtools::MutexLock lock(mutex);
    return workers.size();
  }
}
