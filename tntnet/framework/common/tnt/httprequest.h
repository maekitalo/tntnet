/* tnt/httprequest.h
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

#ifndef TNT_HTTPREQUEST_H
#define TNT_HTTPREQUEST_H

#include <tnt/httpmessage.h>
#include <vector>
#include <netinet/in.h>
#include <tnt/contenttype.h>
#include <tnt/multipart.h>
#include <tnt/cookie.h>
#include <cxxtools/query_params.h>

namespace tnt
{
  /// HTTP-Request-message
  class httpRequest : public httpMessage
  {
    public:
      typedef std::vector<std::string> args_type;

    private:
      std::string pathinfo;
      args_type args;
      cxxtools::query_params qparam;
      struct sockaddr_in peerAddr;
      struct sockaddr_in serverAddr;
      contenttype ct;
      multipart mp;
      bool ssl;
      unsigned serial;
      static unsigned serial_;
      mutable bool lang_init;
      mutable std::string lang;

    public:
      httpRequest()
        : ssl(false),
          lang_init(false)
        { }
      httpRequest(const std::string& url);

      void setPathInfo(const std::string& p)       { pathinfo = p; }
      const std::string& getPathInfo() const       { return pathinfo; }

      void setArgs(const args_type& a)             { args = a; }
      const args_type& getArgs() const             { return args; }
      args_type& getArgs()                         { return args; }

      args_type::const_reference getArgDef(args_type::size_type n,
        const std::string& def = std::string()) const;
      args_type::const_reference getArg(args_type::size_type n) const
                                                   { return args[n]; }
      args_type::size_type getArgsCount() const    { return args.size(); }

      void parse(std::istream& in);
      void doPostParse();

      cxxtools::query_params& getQueryParams()              { return qparam; }
      const cxxtools::query_params& getQueryParams() const  { return qparam; }

      void setPeerAddr(const struct sockaddr_in& p)
        { memcpy(&peerAddr, &p, sizeof(struct sockaddr_in)); }
      const struct sockaddr_in& getPeerAddr() const
        { return peerAddr; }
      std::string getPeerIp() const;

      void setServerAddr(const struct sockaddr_in& p)
        { memcpy(&serverAddr, &p, sizeof(struct sockaddr_in)); }
      const struct sockaddr_in& getServerAddr() const
        { return serverAddr; }
      std::string getServerIp() const;
      unsigned short int getServerPort() const
        { return ntohs(serverAddr.sin_port); }

      void setSsl(bool sw = true)
        { ssl = sw; }
      bool isSsl() const
        { return ssl;  }
      const contenttype& getContentType() const  { return ct; }
      bool isMultipart() const               { return ct.isMultipart(); }
      const multipart& getMultipart() const  { return mp; }

      unsigned getSerial() const             { return serial; }

      std::string getLang() const;

      const cookies& getCookies() const;

      bool hasCookie(const std::string& name) const
        { return getCookies().hasCookie(name); }
      bool hasCookies() const
        { return getCookies().hasCookies(); }
      const cookie& getCookie(const std::string& name) const
        { return getCookies().getCookie(name); }
  };

  inline std::istream& operator>> (std::istream& in, httpRequest& msg)
  { msg.parse(in); return in; }

}

#endif // TNT_HTTPREQUEST_H
