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
#include <cxxtools/log.h>

log_define("tntnet.worker");

namespace
{
  class component_unload_lock
  {
      tnt::component& comp;

    public:
      component_unload_lock(tnt::component& c)
        : comp(c)
      { comp.lock(); }
      ~component_unload_lock()
      { comp.unlock(); }
  };
}

namespace tnt
{
  cxxtools::Mutex worker::mutex;
  unsigned worker::nextThreadNumber = 0;
  worker::workers_type worker::workers;
  unsigned worker::compLifetime = 60;
  unsigned worker::minThreads = 2;

  worker::worker(jobqueue& q, const dispatcher& d,
    poller& p, comploader::load_library_listener* libconfigurator)
    : queue(q),
      mypoller(p),
      ourdispatcher(d),
      threadNumber(++nextThreadNumber)
  {
    log_debug("initialize thread " << threadNumber);
    if (libconfigurator)
      mycomploader.addLoadLibraryListener(libconfigurator);

    cxxtools::MutexLock lock(mutex);
    workers.insert(this);
  }

  worker::~worker()
  {
    cxxtools::MutexLock lock(mutex);
    workers.erase(this);
  }

  void worker::Run()
  {
    log_debug("start thread " << threadNumber);
    while (queue.getWaitThreadCount() < minThreads)
    {
      log_debug("waiting for job");
      jobqueue::job_ptr j = queue.get();
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
            keepAlive = processRequest(
              j->getRequest(), socket, j->decrementKeepAliveCounter());

            if (keepAlive)
              j->clear();
          }
        } while (keepAlive);
      }
      catch (const cxxtools::tcp::Timeout& e)
      {
        log_debug("timeout - put job in poller");
        mypoller.addIdleJob(j);
      }
    }

    log_debug("end worker-thread " << threadNumber);
  }

  bool worker::processRequest(httpRequest& request, std::iostream& socket,
         bool keepAlive)
  {
    // log message
    log_debug("process request: " << request.getMethod() << ' ' << request.getUrl());

    /*
    for (httpMessage::header_type::const_iterator it = request.header_begin();
         it != request.header_end(); ++it)
      log_debug(it->first << ' ' << it->second);
      */

    // create reply-object
    httpReply reply(socket);
    reply.setVersion(request.getMajorVersion(), request.getMinorVersion());
    reply.setMethod(request.getMethod());
    if (keepAlive && request.keepAlive())
      reply.setHeader(httpMessage::Connection, httpMessage::Connection_Keep_Alive);
    else
      reply.setHeader(httpMessage::Connection, httpMessage::Connection_close);

    // process request
    try
    {
      try
      {
        Dispatch(request, reply);
        keepAlive = keepAlive && request.keepAlive() && reply.keepAlive();
        if (keepAlive)
          log_debug("keep alive");
        else
        {
          log_debug("no keep alive request/reply="
              << request.keepAlive() << '/' << reply.keepAlive());
        }
      }
      catch (const cxxtools::dl::dlopen_error& e)
      {
        log_warn("dl::dlopen_error catched");
        throw notFoundException(e.getLibname());
      }
      catch (const cxxtools::dl::symbol_not_found& e)
      {
        log_warn("dl::symbol_not_found catched");
        throw notFoundException(e.getSymbol());
      }
      catch (const httpError& e)
      {
        throw;
      }
      catch (const std::exception& e)
      {
        throw httpError(HTTP_INTERNAL_SERVER_ERROR, e.what());
      }
    }
    catch (const httpError& e)
    {
      log_warn("http-Error: " << e.what());
      socket << "HTTP/1.0 " << e.what()
             << "\r\n\r\n"
             << "<html><body><h1>Error</h1><p>"
             << e.what() << "</p></body></html>" << std::endl;
      return false;
    }
    catch (const std::exception& e)
    {
      log_error(e.what());
      return false;
    }

    return keepAlive;
  }

  void worker::Dispatch(httpRequest& request, httpReply& reply)
  {
    const std::string& url = request.getUrl();

    log_info("dispatch " << request.getQuery());

    if (!httpRequest::checkUrl(url))
      throw httpError(HTTP_BAD_REQUEST, "illegal url");

    dispatcher::pos_type pos(ourdispatcher, request.getUrl());
    while (1)
    {
      dispatcher::compident_type ci = pos.getNext();
      try
      {
        component& comp = mycomploader.fetchComp(ci, ourdispatcher);
        component_unload_lock unload_lock(comp);
        request.setPathInfo(ci.hasPathInfo() ? ci.getPathInfo() : url);
        request.setArgs(ci.getArgs());
        log_debug("call component " << ci
          << " path \"" << ci.getPathInfo() << '"');
        unsigned http_return = comp(request, reply, request.getQueryParams());
        if (http_return != DECLINED)
        {
          if (reply.isDirectMode())
            log_info("request ready, returncode " << http_return);
          else
            log_info("request ready, returncode " << http_return << " - ContentSize: " << reply.getContentSize());

          reply.sendReply(http_return);
          return;
        }
        else
          log_debug("component " << ci << " returned DECLINED");
      }
      catch (const notFoundException&)
      {
      }
    }

    throw notFoundException(request.getUrl());
  }

  void worker::CleanerThread()
  {
    while (1)
    {
      sleep(10);

      log_debug("cleanup");

      {
        cxxtools::MutexLock lock(mutex);
        for (workers_type::iterator it = workers.begin();
             it != workers.end(); ++it)
          (*it)->cleanup(compLifetime);
      }
    }
  }

  worker::workers_type::size_type worker::getCountThreads()
  {
    cxxtools::MutexLock lock(mutex);
    return workers.size();
  }
}
