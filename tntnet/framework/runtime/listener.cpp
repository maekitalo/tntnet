/* listener.cpp
   Copyright (C) 2003 Tommi Maekitalo

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

#include "tnt/listener.h"
#include "tnt/tntnet.h"
#include <cxxtools/log.h>
#include <errno.h>
#include <unistd.h>

#ifdef USE_SSL
#  include "tnt/ssl.h"
#endif

log_define("tntnet.listener");

static void listenRetry(cxxtools::tcp::Server& server,
  const char* ipaddr, unsigned short int port, unsigned retry)
{
  for (unsigned n = 1; true; ++n)
  {
    try
    {
      log_debug("listen " << ipaddr << ':' << port);
      server.Listen(ipaddr, port);
      return;
    }
    catch (const cxxtools::tcp::Exception& e)
    {
      log_debug("cxxtools::tcp::Exception catched: errno=" << e.getErrno() << " msg=" << e.what());
      if (e.getErrno() != EADDRINUSE || n > retry)
      {
        log_debug("rethrow exception");
        throw;
      }
      log_warn("address " << ipaddr << ':' << port << " in use - retry; n = " << n);
      ::sleep(1);
    }
  }
}

namespace tnt
{
  listener::listener(const std::string& ipaddr, unsigned short int port, jobqueue& q)
    : queue(q)
  {
    log_info("listen ip=" << ipaddr << " port=" << port);
    listenRetry(server, ipaddr.c_str(), port, 5);
  }

  void listener::Run()
  {
    // accept-loop
    log_debug("enter accept-loop");
    while (!tntnet::shouldStop())
    {
      try
      {
        tcpjob* j = new tcpjob;
        jobqueue::job_ptr p(j);
        j->Accept(server);
        queue.put(p);
      }
      catch (const std::exception& e)
      {
        log_error("error in accept-loop: " << e.what());
      }
    }
  }

#ifdef USE_SSL
  ssllistener::ssllistener(const char* certificateFile,
      const char* keyFile,
      const std::string& ipaddr, unsigned short int port,
      jobqueue& q)
    : server(certificateFile, keyFile),
      queue(q)
  {
    log_info("listen ip=" << ipaddr << " port=" << port << " (ssl)");
    listenRetry(server, ipaddr.c_str(), port, 5);
  }

  void ssllistener::Run()
  {
    // accept-loop
    log_debug("enter accept-loop (ssl)");
    while (!tntnet::shouldStop())
    {
      try
      {
        ssl_tcpjob* j = new ssl_tcpjob;
        jobqueue::job_ptr p(j);
        j->Accept(server);
        queue.put(p);
      }
      catch (const std::exception& e)
      {
        log_error("error in accept-loop: " << e.what());
      }
    }
  }
#endif // USE_SSL

}
