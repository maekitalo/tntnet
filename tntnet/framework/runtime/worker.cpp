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
#include <cxxtools/md5stream.h>

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
}

namespace tnt
{
  cxxtools::Mutex Worker::mutex;
  unsigned Worker::nextThreadNumber = 0;
  Worker::workers_type Worker::workers;
  unsigned Worker::compLifetime = 600;
  unsigned Worker::minThreads = 5;
  static const std::string& sessionCookiePrefix = "tntnet.";

  Worker::Worker(Tntnet& app)
    : application(app),
      mycomploader(app.getConfig()),
      threadNumber(++nextThreadNumber)
  {
    log_debug("initialize thread " << threadNumber);

    cxxtools::MutexLock lock(mutex);
    workers.insert(this);
  }

  Worker::~Worker()
  {
    cxxtools::MutexLock lock(mutex);
    workers.erase(this);
  }

  void Worker::run()
  {
    Jobqueue& queue = application.getQueue();
    log_debug("start thread " << threadNumber);
    while (queue.getWaitThreadCount() < minThreads)
    {
      log_debug("waiting for job");
      Jobqueue::JobPtr j = queue.get();
      log_debug("got job - fd=" << j->getFd());

      std::iostream& socket = j->getStream();

      try
      {
        bool keepAlive;
        do
        {
          keepAlive = false;
          log_debug("call parser");
          j->getParser().parse(socket);

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
            }
          }
        } while (keepAlive);
      }
      catch (const cxxtools::net::Timeout& e)
      {
        log_debug("timeout - put job in poller");
        application.getPoller().addIdleJob(j);
      }
    }

    log_info("end worker-thread " << threadNumber);
  }

  bool Worker::processRequest(HttpRequest& request, std::iostream& socket,
         unsigned keepAliveCount)
  {
    // log message
    char buffer[20];
    log_debug("process request: " << request.getMethod() << ' ' << request.getUrl()
      << " from client "
      << inet_ntop(AF_INET, &(request.getPeerAddr().sin_addr), buffer, sizeof(buffer)));

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
      catch (const cxxtools::dl::DlopenError& e)
      {
        log_warn("dl::DlopenError catched - libname " << e.getLibname());
        throw NotFoundException(e.getLibname());
      }
      catch (const cxxtools::dl::SymbolNotFound& e)
      {
        log_warn("dl::SymbolNotFound catched - symbol " << e.getSymbol());
        throw NotFoundException(e.getSymbol());
      }
      catch (const HttpError& e)
      {
        throw;
      }
      catch (const std::exception& e)
      {
        throw HttpError(HTTP_INTERNAL_SERVER_ERROR, e.what());
      }
    }
    catch (const HttpError& e)
    {
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
    const std::string& url = request.getUrl();

    log_info("dispatch " << request.getQuery());

    if (!HttpRequest::checkUrl(url))
      throw HttpError(HTTP_BAD_REQUEST, "illegal url");

    std::string currentSessionCookieName;

    Dispatcher::PosType pos(application.getDispatcher(), request.getUrl());
    while (1)
    {
      // pos.getNext() throws NotFoundException at end
      Dispatcher::CompidentType ci = pos.getNext();
      try
      {
        Component& comp = mycomploader.fetchComp(ci, application.getDispatcher());
        ComponentUnloadLock unload_lock(comp);
        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());

        // check session-cookie
        currentSessionCookieName = sessionCookiePrefix + ci.libname;
        Cookie c = request.getCookie(currentSessionCookieName);
        if (c.getValue().empty())
        {
          log_debug("session-cookie " << currentSessionCookieName << " not found");
          request.setSessionScope(0);
        }
        else
        {
          log_debug("session-cookie " << currentSessionCookieName << " found: " << c.getValue());
          Sessionscope* sessionScope = application.getSessionScope(c.getValue());
          if (sessionScope != 0)
          {
            log_debug("session found");
            request.setSessionScope(sessionScope);
          }
        }

        // set application-scope
        request.setApplicationScope(application.getApplicationScope(ci.libname));

        log_debug("call component " << ci
          << " path \"" << ci.getPathInfo() << '"');
        unsigned http_return = comp(request, reply, request.getQueryParams());
        if (http_return != DECLINED)
        {
          if (reply.isDirectMode())
          {
            log_info("request ready, returncode " << http_return);
            reply.out().flush();
          }
          else
          {
            log_info("request ready, returncode " << http_return << " - ContentSize: " << reply.getContentSize());

            if (request.hasSessionScope())
            {
              // request has session-scope
              std::string cookie = request.getCookie(currentSessionCookieName);
              if (cookie.empty() || !application.hasSessionScope(cookie))
              {
                // client has no or unknown cookie

                cxxtools::Md5stream c;
                c << request.getSerial() << '-' << ::pthread_self() << '-' << rand();
                cookie = c.getHexDigest();
                log_debug("set Cookie " << cookie);
                reply.setCookie(currentSessionCookieName, cookie);
                application.putSessionScope(cookie, &request.getSessionScope());
              }
            }
            else
            {
              std::string cookie = request.getCookie(currentSessionCookieName);
              if (!cookie.empty())
              {
                // client has cookie
                log_debug("clear Cookie " << currentSessionCookieName);
                reply.clearCookie(currentSessionCookieName);
                application.removeSessionScope(cookie);
              }
            }

            reply.sendReply(http_return);
            log_debug("reply sent");
          }
          return;
        }
        else
          log_debug("component " << ci << " returned DECLINED");
      }
      catch (const NotFoundException&)
      {
      }
    }

    throw NotFoundException(request.getUrl());
  }

  void Worker::dropOldComponents()
  {
    cxxtools::MutexLock lock(mutex);
    for (workers_type::iterator it = workers.begin();
         it != workers.end(); ++it)
      (*it)->cleanup(compLifetime);
  }

  Worker::workers_type::size_type Worker::getCountThreads()
  {
    cxxtools::MutexLock lock(mutex);
    return workers.size();
  }
}
