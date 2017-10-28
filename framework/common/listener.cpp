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
#include "config.h"

log_define("tntnet.listener")

namespace tnt
{
  namespace
  {
    void doListenRetry(cxxtools::net::TcpServer& server,
      const std::string& ipaddr, unsigned short int port)
    {
      for (unsigned n = 1; true; ++n)
      {
        try
        {
          log_debug("listen " << ipaddr << ':' << port);
#ifdef HAVE_CXXTOOLS_REUSEADDR
          int flags = cxxtools::net::TcpServer::DEFER_ACCEPT;
          if (TntConfig::it().reuseAddress)
            flags |= cxxtools::net::TcpServer::REUSEADDR;

          server.listen(ipaddr, port, TntConfig::it().listenBacklog, flags);
#else
          server.listen(ipaddr, port, TntConfig::it().listenBacklog, cxxtools::net::TcpServer::DEFER_ACCEPT);
#endif
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

  Listener::Listener(Tntnet& application, const std::string& ipaddr, unsigned short int port, Jobqueue& q,
    const std::string& certificateFile, const std::string& privateKeyFile,
    int sslVerifyLevel, const std::string& sslCa)
    : _queue(q),
      _certificateFile(certificateFile),
      _privateKeyFile(privateKeyFile),
      _sslVerifyLevel(sslVerifyLevel),
      _sslCa(sslCa)
  {
    log_info("listen ip=" << ipaddr << " port=" << port);
    doListenRetry(_server, ipaddr, port);
    Jobqueue::JobPtr p = new Tcpjob(application, _server, _queue,
                                    certificateFile, privateKeyFile,
                                    sslVerifyLevel, sslCa);
    _queue.put(p);
  }

  void Listener::terminate()
  {
    log_info("stop listener");
    _server.terminateAccept();
  }

}
