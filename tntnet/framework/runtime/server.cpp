/* server.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include "tnt/server.h"
#include "tnt/dispatcher.h"
#include "tnt/job.h"
#include <tnt/http.h>
#include <tnt/log.h>

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
  Mutex server::mutex;
  unsigned server::nextThreadNumber = 0;
  server::servers_type server::servers;
  unsigned server::compLifetime = 60;

  log_define_class(server, "tntnet.server");

  server::server(jobqueue& q, const dispatcher& d,
    comploader::load_library_listener* libconfigurator)
    : queue(q),
      ourdispatcher(d),
      threadNumber(++nextThreadNumber)
  {
    log_debug("initialize thread " << threadNumber);
    if (libconfigurator)
      mycomploader.addLoadLibraryListener(libconfigurator);
    MutexLock lock(mutex);
    servers.insert(this);
  }

  void server::Run()
  {
    log_debug("start thread " << threadNumber);
    while (1)
    {
      jobqueue::job_ptr j = queue.get();
      std::iostream& socket = j->getStream();

      try
      {
        try
        {
          bool keepAlive;
          do
          {
            keepAlive = false;
            httpRequest request;
            socket >> request;

            if (!socket.good())
            {
              if (socket.fail())
                log_warn("stream error");
              else if (socket.eof())
                log_debug("eof");
              break;
            }

            log_debug("request: " << request.getMethod() << ' ' << request.getUrl());
            for (httpMessage::header_type::const_iterator it = request.header_begin();
                 it != request.header_end(); ++it)
              log_debug(it->first << ' ' << it->second);

            request.setPeerAddr(j->getPeeraddr_in());
            request.setServerAddr(j->getServeraddr_in());
            request.setSsl(j->isSsl());

            httpReply reply(socket);
            reply.setVersion(request.getMajorVersion(), request.getMinorVersion());

            try
            {
              Dispatch(request, reply);
              keepAlive = socket
                       && request.keepAlive()
                       && reply.keepAlive();
              if (keepAlive)
                log_debug("keep alive");
              else
                log_debug("no keep alive request/reply="
                    << request.keepAlive() << '/' << reply.keepAlive());
            }
            catch (const dl::dlopen_error& e)
            {
              log_warn("dl::dlopen_error catched");
              throw notFoundException(e.getLibname());
            }
            catch (const dl::symbol_not_found& e)
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

          } while (keepAlive);
        }
        catch (const httpError& e)
        {
          log_warn("http-Error: " << e.what());
          socket << "HTTP/1.0 " << e.what()
                 << "\r\n\r\n"
                 << "<html><body><h1>Error</h1><p>"
                 << e.what() << "</p></body></html>" << std::endl;
        }
      }
      catch (const tcp::Timeout& e)
      {
        log_warn("Timout");
      }
      catch (const std::exception& e)
      {
        log_error(e.what());
      }
    }

    log_debug("stop server thread");
    MutexLock lock(mutex);
    servers.erase(this);
  }

  void server::Dispatch(httpRequest& request, httpReply& reply)
  {
    const std::string& url = request.getUrl();

    log_info("dispatch " << url);

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

  void server::CleanerThread()
  {
    while (1)
    {
      sleep(10);

      log_debug("cleanup");

      {
        MutexLock lock(mutex);
        for (servers_type::iterator it = servers.begin();
             it != servers.end(); ++it)
          (*it)->cleanup(compLifetime);
      }
    }
  }
}
