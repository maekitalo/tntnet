/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#include "tnt/listener.h"
#include "tnt/tntnet.h"
#include "tnt/job.h"
#include <cxxtools/log.h>
#include <cxxtools/net/net.h>
#include <unistd.h>

#ifdef WITH_GNUTLS
#  include "tnt/gnutls.h"
#endif

#ifdef WITH_OPENSSL
#  include "tnt/openssl.h"
#endif

log_define("tntnet.listener")

namespace tnt
{
  namespace
  {
    void doListenRetry(cxxtools::net::TcpServer& server,
      const char* ipaddr, unsigned short int port)
    {
      for (unsigned n = 1; true; ++n)
      {
        try
        {
          log_debug("listen " << ipaddr << ':' << port);
          server.listen(ipaddr, port, TntConfig::it().listenBacklog, cxxtools::net::TcpServer::DEFER_ACCEPT);
          return;
        }
        catch (const cxxtools::net::AddressInUse& e)
        {
          log_debug("cxxtools::net::AddressInUse");
          if (n > TntConfig::it().listenRetry)
          {
            log_debug("rethrow exception");
            throw;
          }
          log_warn("address " << ipaddr << ':' << port << " in use - retry; n = " << n);
          ::sleep(1);
        }
      }
    }
  }

  void ListenerBase::terminate()
  {
    log_info("stop listener " << ipaddr << ':' << port);
    doTerminate();
  }

  void ListenerBase::initialize()
  {
  }

  Listener::Listener(Tntnet& application, const std::string& ipaddr_, unsigned short int port_, Jobqueue& q)
    : ListenerBase(ipaddr_, port_),
      queue(q)
  {
    doListenRetry(server, ipaddr_.c_str(), port_);
    queue.put(new Tcpjob(application, server, queue));
  }

  void Listener::doTerminate()
  {
    server.terminateAccept();
  }

  void Listener::initialize()
  {
    log_info("listen ip=" << getIpaddr() << " port=" << getPort());
  }

#ifdef WITH_GNUTLS
#define USE_SSL

#endif

#ifdef WITH_OPENSSL
#define USE_SSL

#endif

#ifdef USE_SSL
  Ssllistener::Ssllistener(Tntnet& application, const char* certificateFile,
      const char* keyFile,
      const std::string& ipaddr_, unsigned short int port_,
      Jobqueue& q)
    : ListenerBase(ipaddr_, port_),
      server(certificateFile, keyFile),
      queue(q)
  {
    doListenRetry(server, ipaddr_.c_str(), port_);
    queue.put(new SslTcpjob(application, server, queue));
  }

  void Ssllistener::doTerminate()
  {
    server.terminateAccept();
  }

  void Ssllistener::initialize()
  {
    log_info("listen ip=" << getIpaddr() << " port=" << getPort() << " (ssl)");
  }

#endif // USE_SSL

}
