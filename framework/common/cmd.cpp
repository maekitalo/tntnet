/*
 * Copyright (C) 2010 Tommi Maekitalo
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


#include <tnt/cmd.h>
#include <cxxtools/sslcertificate.h>
#include <cxxtools/log.h>

log_define("tntnet.cmd")

namespace tnt
{
  namespace
  {
    // SocketIf methods
    class NullSocketIf : public SocketIf
    {
        bool _ssl;

      public:
        explicit NullSocketIf(bool ssl)
          : _ssl(ssl)
          { }

        std::string getPeerIp() const   { return std::string(); }
        std::string getServerIp() const { return std::string(); }
        bool isSsl() const              { return _ssl; }
        cxxtools::SslCertificate getSslCertificate() const
          { return cxxtools::SslCertificate(); }
    };

    NullSocketIf socketIf(false);
  }

  Cmd::Cmd(std::ostream& out)
    : _request(_application, &socketIf),
      _reply(out, false)
  {
    _reply.setDirectModeNoFlush();
  }

  void Cmd::call(const Compident& ci, const QueryParams& q)
  {
    _request.setQueryString(q.getUrl());
    call(ci);
  }

  void Cmd::call(const Compident& ci)
  {
    log_debug("call " << ci);

    _request.doPostParse();

    // set thread context for thread scope
    log_debug("set thread context");
    _request.setThreadContext(&threadContext);
    if (!_sessionId.empty())
    {
      std::string cookieName;
      if (ci.libname.empty())
        cookieName = "tntnet";
      else
      {
        cookieName = "tntnet.";
        cookieName.append(ci.libname);
      }

      Cookies c;
      c.setCookie(cookieName, _sessionId);
      std::ostringstream s;
      s << c;
      _request.setHeader(httpheader::cookie, s.str());
    }

    // sets session and application scope
    log_debug("set session and application scope; session id=<" << _sessionId << '>');
    _scopeManager.preCall(_request, ci.libname);
    _scopeManager.setSessionId(_request, _sessionId);

    // fetch and call the component
    log_debug("do call");
    _comploader.fetchComp(ci)(_request, _reply, _request.getQueryParams());

    // sets session cookie if needed
    _sessionId = _scopeManager.postCall(_request, _reply, _application.getAppName());
    log_debug("session id = " << _sessionId);
    _request.clear();
  }

}
