/* listener.cpp
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

#include "tnt/listener.h"
#include "tnt/ssl.h"
#include "tnt/log.h"
#include "tnt/tntnet.h"

namespace tnt
{
  listener::listener(const std::string& ipaddr, unsigned short int port, jobqueue& q)
    : server(ipaddr, port),
      queue(q)
  {
    log_debug("Listen to " << ipaddr << " port " << port);
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

  ssllistener::ssllistener(const char* certificateFile,
      const char* keyFile,
      const std::string& ipaddr, unsigned short int port,
      jobqueue& q)
    : server(certificateFile, keyFile),
      queue(q)
  {
    log_debug("Listen to " << ipaddr << " port " << port << " (ssl)");
    server.Listen(ipaddr.c_str(), port);
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
}
