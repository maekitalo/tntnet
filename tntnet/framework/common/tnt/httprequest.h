/* tnt/httprequest.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_HTTPREQUEST_H
#define TNT_HTTPREQUEST_H

#include <tnt/httpmessage.h>
#include <vector>
#include <netinet/in.h>
#include <tnt/contenttype.h>
#include <tnt/multipart.h>
#include <tnt/cookie.h>
#include <tnt/encoding.h>
#include <cxxtools/query_params.h>
#include <tnt/scope.h>
#include <locale>
#include <sys/socket.h>

namespace tnt
{
  class Sessionscope;

  /// HTTP-Request-message
  class HttpRequest : public HttpMessage
  {
    public:
      class Parser;
      friend class Parser;

      typedef std::vector<std::string> args_type;

    private:
      std::string body;
      std::string method;
      std::string url;
      std::string queryString;

      size_t contentSize;
      static size_t maxRequestSize;

      std::string pathinfo;
      args_type args;
      cxxtools::QueryParams qparam;

      struct sockaddr_storage peerAddr;
      struct sockaddr_storage serverAddr;

      Contenttype ct;
      Multipart mp;
      bool ssl;
      unsigned serial;
      static unsigned serial_;
      mutable bool locale_init;
      mutable std::locale locale;

      mutable Encoding encoding;
      mutable bool encodingRead;

      mutable std::string username;
      mutable std::string password;

      Scope* requestScope;
      Scope* applicationScope;
      Scope* threadScope;
      Sessionscope* sessionScope;

      bool applicationScopeLocked;
      bool sessionScopeLocked;

      void ensureApplicationScopeLock();
      void ensureSessionScopeLock();

      void releaseApplicationScopeLock();
      void releaseSessionScopeLock();

      void releaseLocks() { releaseSessionScopeLock(); }

      mutable std::string peerAddrStr;
      mutable std::string serverAddrStr;

    public:
      HttpRequest();
      HttpRequest(const std::string& url);
      HttpRequest(const HttpRequest& r);
      ~HttpRequest();

      HttpRequest& operator= (const HttpRequest& r);

      void clear();

      /// returns the body of the message.
      const std::string& getBody() const        { return body; }
      /// sets the body of the message.
      void setBody(const std::string& body_)    { body = body_; }
      /// Returns the http-method (normally GET or POST) of a request.
      const std::string& getMethod() const      { return method; }
      /// sets the http-method of this request.
      void setMethod(const std::string& m)      { method = m; }

      /// returns url with get-parameters.
      std::string getQuery() const
        { return queryString.empty() ? url : url + '?' + queryString; }
      /// returns the request-url without parameters.
      const std::string& getUrl() const         { return url; }
      /// returns get-parameters as string.
      const std::string& getQueryString() const { return queryString; }
      /// sets query-string
      void setQueryString(const std::string& queryString_)
        { queryString = queryString_; }

      void setPathInfo(const std::string& p)       { pathinfo = p; }
      const std::string& getPathInfo() const       { return pathinfo; }

      void setArgs(const args_type& a)             { args = a; }
      const args_type& getArgs() const             { return args; }
      args_type& getArgs()                         { return args; }

      args_type::const_reference getArgDef(args_type::size_type n,
        const std::string& def = std::string()) const
        { return args.size() > n ? args[n] : def; }
      args_type::const_reference getArg(args_type::size_type n) const
                                                   { return args[n]; }
      args_type::size_type getArgsCount() const    { return args.size(); }

      void parse(std::istream& in);
      void doPostParse();

      cxxtools::QueryParams& getQueryParams()               { return qparam; }
      const cxxtools::QueryParams& getQueryParams() const   { return qparam; }

      void setPeerAddr(const struct sockaddr_storage& p)
        { memcpy(&peerAddr, &p, sizeof(peerAddr)); peerAddrStr.clear(); }
      const struct sockaddr_storage& getPeerAddr() const
        { return peerAddr; }
      std::string getPeerIp() const;

      void setServerAddr(const struct sockaddr_storage& p)
        { memcpy(&serverAddr, &p, sizeof(serverAddr)); serverAddrStr.clear(); }
      const struct sockaddr_storage& getServerAddr() const
        { return serverAddr; }
      std::string getServerIp() const;
      unsigned short int getServerPort() const
        { return ntohs(reinterpret_cast <const struct sockaddr_in *> (&serverAddr)->sin_port); }

      void setSsl(bool sw = true)
        { ssl = sw; }
      bool isSsl() const
        { return ssl;  }
      const Contenttype& getContentType() const  { return ct; }
      bool isMultipart() const               { return ct.isMultipart(); }
      const Multipart& getMultipart() const  { return mp; }

      unsigned getSerial() const             { return serial; }

      const std::locale& getLocale() const;
      std::string getLang() const  { return getLocale().name(); }

      const Cookies& getCookies() const;

      bool hasCookie(const std::string& name) const
        { return getCookies().hasCookie(name); }
      bool hasCookies() const
        { return getCookies().hasCookies(); }
      Cookie getCookie(const std::string& name) const
        { return getCookies().getCookie(name); }

      const Encoding& getEncoding() const;
      std::string getUserAgent() const
        { return getHeader(httpheader::userAgent); }
      std::string getHost() const
        { return getHeader(httpheader::host); }

      const std::string& getUsername() const;
      const std::string& getPassword() const;
      bool verifyPassword(const std::string& password) const;

      bool keepAlive() const;

      void setApplicationScope(Scope* s);
      void setApplicationScope(Scope& s)  { setApplicationScope(&s); }

      void setThreadScope(Scope* s);
      void setThreadScope(Scope& s)       { setThreadScope(&s); }

      void setSessionScope(Sessionscope* s);
      void setSessionScope(Sessionscope& s)      { setSessionScope(&s); }
      void clearSession();

      Scope& getRequestScope();
      Scope& getApplicationScope();
      Scope& getThreadScope();
      Sessionscope& getSessionScope();
      bool   hasSessionScope() const;

      /// returns the value of the content-size-header as read from the client.
      size_t getContentSize() const
        { return contentSize; }

      /// returns the virtual-host-header of this request.
      std::string getVirtualHost() const
        { return getHeader(httpheader::host); }

      /// Sets a limit for a maximum request size.
      static void setMaxRequestSize(size_t s)    { maxRequestSize = s; }
      static size_t getMaxRequestSize()          { return maxRequestSize; }
  };

  inline std::istream& operator>> (std::istream& in, HttpRequest& msg)
  { msg.parse(in); return in; }

}

#endif // TNT_HTTPREQUEST_H
