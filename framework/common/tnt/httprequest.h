/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#ifndef TNT_HTTPREQUEST_H
#define TNT_HTTPREQUEST_H

#include <tnt/httpmessage.h>
#include <tnt/httpheader.h>
#include <tnt/socketif.h>
#include <tnt/contenttype.h>
#include <tnt/multipart.h>
#include <tnt/cookie.h>
#include <tnt/encoding.h>
#include <tnt/query_params.h>
#include <tnt/scope.h>
#include <tnt/threadcontext.h>
#include <map>
#include <cxxtools/atomicity.h>
#include <cxxtools/sslcertificate.h>
#include <string>
#include <cstring>

namespace tnt
{
  class Sessionscope;
  class Tntnet;

  /// HTTP request message
  class HttpRequest : public HttpMessage
  {
      friend class SessionUnlocker;
      friend class ApplicationUnlocker;

    public:
      // forward declaration of subclass defined in httpparser.h
      class Parser;

      typedef std::map<std::string, std::string> args_type;

    private:
      std::string _body;
      unsigned _methodLen;
      char _method[8];
      std::string _url;
      std::string _queryString;

      size_t _contentSize;

      std::string _pathinfo;
      args_type _args;
      tnt::QueryParams _getparam;
      tnt::QueryParams _postparam;
      tnt::QueryParams _qparam;

      const SocketIf* _socketIf;

      mutable Contenttype _ct;
      Multipart _mp;
      cxxtools::atomic_t _serial;
      static cxxtools::atomic_t _nextSerial;

      mutable Encoding _encoding;
      mutable bool _encodingRead;

      mutable std::string _username;
      mutable std::string _password;

      Scope* _requestScope;
      Scope* _applicationScope;
      Sessionscope* _sessionScope;
      Sessionscope* _secureSessionScope;
      ThreadContext* _threadContext;

      bool _applicationScopeLocked;
      bool _sessionScopeLocked;
      bool _secureSessionScopeLocked;

      mutable std::string _peerAddrStr;
      mutable std::string _serverAddrStr;

      Tntnet& _application;

      void ensureApplicationScopeLock();
      void ensureSessionScopeLock();

      void releaseApplicationScopeLock();
      void releaseSessionScopeLock();

      void releaseLocks() { releaseApplicationScopeLock(); }

      const Contenttype& getContentTypePriv() const;

    public:
      explicit HttpRequest(Tntnet& application, const SocketIf* socketIf = 0);
      HttpRequest(Tntnet& application, const std::string& url, const SocketIf* socketIf = 0);
      HttpRequest(const HttpRequest& r);
      ~HttpRequest();

      HttpRequest& operator= (const HttpRequest& r);

      void clear();

      /// Get the body of the message
      const std::string& getBody() const        { return _body; }

      /// Set the body of the message
      void setBody(const std::string& body)     { _body = body; }

      /// @{
      /// Get the http method of a request (usually GET or POST)
      std::string getMethod() const             { return _method; }
      const char* getMethod_cstr() const        { return _method; }
      /// @}

      /// Check whether http method used is GET
      bool isMethodGET() const                  { return std::strcmp(_method, "GET") == 0; }

      /// Check whether http method used is POST
      bool isMethodPOST() const                 { return std::strcmp(_method, "POST") == 0; }

      /// Check whether http method used is HEAD
      bool isMethodHEAD() const                 { return std::strcmp(_method, "HEAD") == 0; }

      /// Set the http method of this request
      void setMethod(const char* _method);

      /// Get url with GET parameters
      std::string getQuery() const
        { return _queryString.empty() ? _url : _url + '?' + _queryString; }

      /// Get the request url without GET parameters
      const std::string& getUrl() const         { return _url; }

      /// Get the query string (GET parameters string)
      const std::string& getQueryString() const { return _queryString; }

      /// Set the query string
      void setQueryString(const std::string& queryString)
        { _queryString = queryString; }

      void setPathInfo(const std::string& p)    { _pathinfo = p; }
      const std::string& getPathInfo() const    { return _pathinfo; }

      void setArgs(const args_type& a, bool addToQparam = true);
      const args_type& getArgs() const          { return _args; }
      args_type& getArgs()                      { return _args; }

      /// @{
      /// @deprecated
      std::string getArgDef(args_type::size_type n, const std::string& def = std::string()) const;
      std::string getArg(args_type::size_type n) const { return getArgDef(n); }
      args_type::size_type getArgsCount() const { return _args.size(); }
      /// @}

      std::string getArg(const std::string& name, const std::string& def = std::string()) const;

      void parse(std::istream& in);
      void doPostParse();

      /// @{
      /// Get query parameters (GET and POST)
      tnt::QueryParams& getQueryParams()             { return _qparam; }
      const tnt::QueryParams& getQueryParams() const { return _qparam; }
      /// @}

      /// Get GET parameters
      const tnt::QueryParams& getGetParams() const   { return _getparam; }

      /// Get POST parameters
      const tnt::QueryParams& getPostParams() const  { return _postparam; }

      /// Set query parameters (GET and POST)
      void setQueryParams(const tnt::QueryParams& q) { _qparam = q; }

      /// Get the IP the request was sent from
      std::string getPeerIp() const   { return _socketIf ? _socketIf->getPeerIp()   : std::string(); }

      cxxtools::SslCertificate getSslCertificate() const
      {
        cxxtools::SslCertificate ret;
        if (_socketIf)
          ret = _socketIf->getSslCertificate();
        return ret;
      }

      /// Get the IP the request was sent to
      std::string getServerIp() const { return _socketIf ? _socketIf->getServerIp() : std::string(); }

      /// Check whether the request was sent over an SSL (https) connection
      bool isSsl() const              { return _socketIf && _socketIf->isSsl(); }

      const Contenttype& getContentType() const
        { return _ct.getType().empty() && hasHeader(httpheader::contentType) ? getContentTypePriv() : _ct; }
      bool isMultipart() const              { return getContentType().isMultipart(); }
      const Multipart& getMultipart() const { return _mp; }

      cxxtools::atomic_t getSerial() const  { return _serial; }

      const Cookies& getCookies() const;

      bool hasCookie(const std::string& name) const
        { return getCookies().hasCookie(name); }
      bool hasCookies() const
        { return getCookies().hasCookies(); }
      Cookie getCookie(const std::string& name) const
        { return getCookies().getCookie(name); }

      const Encoding& getEncoding() const;

      /// Get the user agent (webbrowser) HTTP header
      const char* getUserAgent() const
        { return getHeader(httpheader::userAgent); }

      /// Get the host (operating system) HTTP header
      const char* getHost() const
        { return getHeader(httpheader::host); }

			/// Get the HTTP-Auth username
      const std::string& getUsername() const;

      /// Get the HTTP-Auth password
      const std::string& getPassword() const;

      /// Check equality of the HTTP-Auth password and the parameter
      bool verifyPassword(const std::string& password) const;

      bool keepAlive() const;

      /// Check whether the client accepts gzip compression
      bool acceptGzipEncoding() const { return getEncoding().accept("gzip"); }

      void setApplicationScope(Scope* s);
      void setApplicationScope(Scope& s) { setApplicationScope(&s); }

      void setSessionScope(Sessionscope* s);
      void setSessionScope(Sessionscope& s) { setSessionScope(&s); }
      void setSecureSessionScope(Sessionscope* s);
      void setSecureSessionScope(Sessionscope& s) { setSecureSessionScope(&s); }

      void setThreadContext(ThreadContext* ctx) { _threadContext = ctx; }

      Scope& getRequestScope();
      Scope& getApplicationScope();
      Scope& getThreadScope();
      Sessionscope& getSessionScope();
      Sessionscope& getSecureSessionScope();
      bool hasSessionScope() const;
      bool hasSecureSessionScope() const;

      /// Get the value of the content-size HTTP header
      size_t getContentSize() const
        { return _contentSize; }

      /// Get the virtual-host HTTP header
      std::string getVirtualHost() const
        { return getHeader(httpheader::host); }

      Tntnet& getApplication()
        { return _application; }

      /// Rewind watchdog timer
      void touch() { _threadContext->touch(); }

      static void postRunCleanup();
  };

  inline std::istream& operator>> (std::istream& in, HttpRequest& msg)
    { msg.parse(in); return in; }
}

#endif // TNT_HTTPREQUEST_H

