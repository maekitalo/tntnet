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


#ifndef TNT_LISTENER_H
#define TNT_LISTENER_H

#include <config.h>
#include "tnt/tcpjob.h"
#include <cxxtools/net/tcpserver.h>

namespace tnt
{
  class Listener
  {
      cxxtools::net::TcpServer _server;
      Jobqueue& _queue;

      std::string _certificateFile;
      std::string _privateKeyFile;
      int _sslVerifyLevel;
      std::string _sslCa;

    public:
      Listener(Tntnet& application, const std::string& ipaddr, unsigned short int port, Jobqueue& q,
        const std::string& certificateFile = std::string(), const std::string& privateKeyFile = std::string(),
        int sslVerifyLevel = 0, const std::string& sslCa = std::string());

      void terminate();
  };

}

#endif // TNT_LISTENER_H

