/*
 * Copyright (C) 2007 Tommi Maekitalo
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


#ifndef TNT_TCPJOB_H
#define TNT_TCPJOB_H

#include <tnt/job.h>
#include <cxxtools/net/tcpstream.h>
#include <tnt/socketif.h>
#include <tnt/tntconfig.h>

/// @cond internal

namespace tnt
{
  class Tcpjob : public Job, private SocketIf
  {
      cxxtools::net::TcpStream _socket;
      const cxxtools::net::TcpServer& _listener;
      Jobqueue& _queue;

      bool _sslInitialized;
      std::string _certificateFile;
      std::string _privateKeyFile;
      int _sslVerifyLevel;
      std::string _sslCa;

      void accept();
      void regenerateJob();

      virtual std::string getPeerIp() const;
      virtual std::string getServerIp() const;
      virtual bool isSsl() const;
      virtual cxxtools::SslCertificate getSslCertificate() const;

    public:
      Tcpjob(Tntnet& app, const cxxtools::net::TcpServer& listener, Jobqueue& queue,
        const std::string& certificateFile, const std::string& privateKeyFile,
        int sslVerifyLevel, const std::string& sslCa)
        : Job(app, this),
          _socket(TntConfig::it().socketBufferSize, TntConfig::it().socketReadTimeout),
          _listener(listener),
          _queue(queue),
          _sslInitialized(false),
          _certificateFile(certificateFile),
          _privateKeyFile(privateKeyFile),
          _sslVerifyLevel(sslVerifyLevel),
          _sslCa(sslCa)
        { }

      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };
}

#endif // TNT_TCPJOB_H

