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
#include <locale>
#include <map>
#include <cxxtools/atomicity.h>
#include <string>
#include <cstring>

namespace tnt
{
  class Sessionscope;
  class Tntnet;

  /// HTTP request message
  class HttpRequest : public HttpMessage
  {
    public:
      class Parser;
      friend class Parser;
      friend class SessionUnlocker;
      friend class ApplicationUnlocker;

      typedef std::map<std::string, std::string> args_type;

    private:
      std::string _body;
      unsigned _methodLen;
      char method[8];
      std::string url;
      std::string queryString;

      size_t contentSize;

      std::string pathinfo;
      args_type args;
      tnt::QueryParams getparam;
      tnt::QueryParams postparam;
      tnt::QueryParams qparam;

      const SocketIf* socketIf;

      mutable Contenttype ct;
      Multipart mp;
      cxxtools::atomic_t serial;
      static cxxtools::atomic_t serial_;
      mutable bool locale_init;
      mutable std::string lang;

      mutable Encoding encoding;
      mutable bool encodingRead;

      mutable std::string username;
      mutable std::string password;

      Scope* requestScope;
      Scope* applicationScope;
      Sessionscope* sessionScope;
      Sessionscope* secureSessionScope;
      ThreadContext* threadContext;

      bool applicationScopeLocked;
      bool sessionScopeLocked;
      bool secureSessionScopeLocked;

      void ensureApplicationScopeLock();
      void ensureSessionScopeLock();

      void releaseApplicationScopeLock();
      void releaseSessionScopeLock();

      void releaseLocks() { releaseApplicationScopeLock(); }

      mutable std::string peerAddrStr;
      mutable std::string serverAddrStr;

      const Contenttype& getContentTypePriv() const;
      Tntnet& application;

    public:
      explicit HttpRequest(Tntnet& application_, const SocketIf* socketIf = 0);
      HttpRequest(Tntnet& application_, const std::string& url, const SocketIf* socketIf = 0);
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
      std::string getMethod() const             { return method; }
      const char* getMethod_cstr() const        { return method; }
      /// @}

      /// Check whether http method used is GET
      bool isMethodGET() const                  { return std::strcmp(method, "GET") == 0; }

      /// Check whether http method used is POST
      bool isMethodPOST() const                 { return std::strcmp(method, "POST") == 0; }

      /// Check whether http method used is HEAD
      bool isMethodHEAD() const                 { return std::strcmp(method, "HEAD") == 0; }

      /// Set the http method of this request
      void setMethod(const char* method);

      /// Get url with GET parameters
      std::string getQuery() const
        { return queryString.empty() ? url : url + '?' + queryString; }

      /// Get the request url without GET parameters
      const std::string& getUrl() const         { return url; }

      /// Get the query string (GET parameters string)
      const std::string& getQueryString() const { return queryString; }

      /// Set the query string
      void setQueryString(const std::string& queryString_)
        { queryString = queryString_; }

      void setPathInfo(const std::string& p)       { pathinfo = p; }
      const std::string& getPathInfo() const       { return pathinfo; }

      void setArgs(const args_type& a)             { args = a; }
      const args_type& getArgs() const             { return args; }
      args_type& getArgs()                         { return args; }

      /// @{
      /// @deprecated
      std::string getArgDef(args_type::size_type n, const std::string& def = std::string()) const;
      std::string getArg(args_type::size_type n) const { return getArgDef(n); }
      args_type::size_type getArgsCount() const { return args.size(); }
      /// @}

      std::string getArg(const std::string& name, const std::string& def = std::string()) const;

      void parse(std::istream& in);
      void doPostParse();

      tnt::QueryParams& getQueryParams()               { return qparam; }
      const tnt::QueryParams& getQueryParams() const   { return qparam; }
      const tnt::QueryParams& getGetParams() const     { return getparam; }
      const tnt::QueryParams& getPostParams() const    { return postparam; }
      void setQueryParams(const tnt::QueryParams& q)   { qparam = q; }

      std::string getPeerIp() const    { return socketIf ? socketIf->getPeerIp()   : std::string(); }
      std::string getServerIp() const  { return socketIf ? socketIf->getServerIp() : std::string(); }
      bool isSsl() const  { return socketIf && socketIf->isSsl(); }

      const Contenttype& getContentType() const
        { return ct.getType().empty() && hasHeader(httpheader::contentType) ? getContentTypePriv() : ct; }
      bool isMultipart() const               { return getContentType().isMultipart(); }
      const Multipart& getMultipart() const  { return mp; }

      cxxtools::atomic_t getSerial() const   { return serial; }

      const std::locale& getLocale() const;
      const std::string& getLang() const
      {
        if (!locale_init)
          getLocale();
        return lang;
      }

      void setLocale(const std::locale& loc);
      void setLang(const std::string& lang);

      const Cookies& getCookies() const;

      bool hasCookie(const std::string& name) const
        { return getCookies().hasCookie(name); }
      bool hasCookies() const
        { return getCookies().hasCookies(); }
      Cookie getCookie(const std::string& name) const
        { return getCookies().getCookie(name); }

      const Encoding& getEncoding() const;
      const char* getUserAgent() const
        { return getHeader(httpheader::userAgent); }
      const char* getHost() const
        { return getHeader(httpheader::host); }

      const std::string& getUsername() const;
      const std::string& getPassword() const;
      bool verifyPassword(const std::string& password) const;

      bool keepAlive() const;
      
      /// @return Whether the client accepts gzip compression
      bool acceptGzipEncoding() const { return getEncoding().accept("gzip"); }

      void setApplicationScope(Scope* s);
      void setApplicationScope(Scope& s) { setApplicationScope(&s); }

      void setSessionScope(Sessionscope* s);
      void setSessionScope(Sessionscope& s) { setSessionScope(&s); }
      void setSecureSessionScope(Sessionscope* s);
      void setSecureSessionScope(Sessionscope& s) { setSecureSessionScope(&s); }

      void setThreadContext(ThreadContext* ctx) { threadContext = ctx; }

      Scope& getRequestScope();
      Scope& getApplicationScope();
      Scope& getThreadScope();
      Sessionscope& getSessionScope();
      Sessionscope& getSecureSessionScope();
      bool   hasSessionScope() const;
      bool   hasSecureSessionScope() const;

      /// Get the value of the content-size header as reported by the client
      size_t getContentSize() const
        { return contentSize; }

      /// Get the virtual-host header of this request
      std::string getVirtualHost() const
        { return getHeader(httpheader::host); }

      Tntnet& getApplication()
        { return application; }

      /// Rewind watchdog timer
      void touch() { threadContext->touch(); }

      static void postRunCleanup();
  };

  inline std::istream& operator>> (std::istream& in, HttpRequest& msg)
    { msg.parse(in); return in; }

}

#endif // TNT_HTTPREQUEST_H
