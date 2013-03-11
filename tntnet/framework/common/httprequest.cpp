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


#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <tnt/httperror.h>
#include <tnt/util.h>
#include <sstream>
#include <cxxtools/log.h>
#include <cxxtools/mutex.h>
#include <cxxtools/base64stream.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tnt/sessionscope.h>
#include <tnt/socketif.h>
#include <pthread.h>
#include "config.h"
#include <tnt/stringlessignorecase.h>
#include <cstring>

namespace tnt
{
  log_define("tntnet.httprequest")

  ////////////////////////////////////////////////////////////////////////
  // HttpRequest
  //
  cxxtools::atomic_t HttpRequest::serial_ = 0;

  HttpRequest::HttpRequest(Tntnet& application_, const SocketIf* socketIf_)
    : socketIf(socketIf_),
      locale_init(false),
      encodingRead(false),
      requestScope(0),
      applicationScope(0),
      sessionScope(0),
      secureSessionScope(0),
      threadContext(0),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      secureSessionScopeLocked(false),
      application(application_)
  {
  }

  HttpRequest::HttpRequest(Tntnet& application_, const std::string& url_, const SocketIf* socketIf_)
    : socketIf(socketIf_),
      locale_init(false),
      requestScope(0),
      applicationScope(0),
      sessionScope(0),
      secureSessionScope(0),
      threadContext(0),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      secureSessionScopeLocked(false),
      application(application_)
  {
    std::istringstream s("GET " + url_ + " HTTP/1.1\r\n\r\n");
    parse(s);
  }

  HttpRequest::HttpRequest(const HttpRequest& r)
    : methodLen(0),
      pathinfo(r.pathinfo),
      args(r.args),
      getparam(r.getparam),
      postparam(r.postparam),
      qparam(r.qparam),
      socketIf(r.socketIf),
      ct(r.ct),
      mp(r.mp),
      serial(r.serial),
      locale_init(r.locale_init),
      locale(r.locale),
      requestScope(r.requestScope),
      applicationScope(r.applicationScope),
      sessionScope(r.sessionScope),
      secureSessionScope(r.secureSessionScope),
      threadContext(r.threadContext),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      secureSessionScopeLocked(false),
      application(r.application)
  {
    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (sessionScope)
      sessionScope->addRef();
    if (secureSessionScope)
      secureSessionScope->addRef();
  }

  HttpRequest::~HttpRequest()
  {
    releaseLocks();

    if (requestScope && requestScope->release() == 0)
      delete requestScope;
    if (applicationScope && applicationScope->release() == 0)
      delete applicationScope;
    if (sessionScope && sessionScope->release() == 0)
      delete sessionScope;
    if (secureSessionScope && secureSessionScope->release() == 0)
      delete secureSessionScope;
  }

  HttpRequest& HttpRequest::operator= (const HttpRequest& r)
  {
    pathinfo = r.pathinfo;
    args = r.args;
    getparam = r.getparam;
    postparam = r.postparam;
    qparam = r.qparam;
    ct = r.ct;
    mp = r.mp;
    socketIf = r.socketIf;
    serial = r.serial;
    locale_init = r.locale_init;
    locale = r.locale;
    requestScope = r.requestScope;
    applicationScope = r.applicationScope;
    sessionScope = r.sessionScope;
    secureSessionScope = r.secureSessionScope;
    threadContext = r.threadContext;
    applicationScopeLocked = false;
    sessionScopeLocked = false;
    secureSessionScopeLocked = false;

    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (sessionScope)
      sessionScope->addRef();
    if (secureSessionScope)
      secureSessionScope->addRef();

    return *this;
  }

  void HttpRequest::clear()
  {
    HttpMessage::clear();
    body.clear();
    methodLen = 0;
    method[0] = '\0';
    url.clear();
    queryString.clear();
    contentSize = 0;
    pathinfo.clear();
    args.clear();
    getparam.clear();
    postparam.clear();
    qparam.clear();
    ct = Contenttype();
    mp = Multipart();
    locale_init = false;
    if (requestScope)
    {
      if (requestScope->release() == 0)
        delete requestScope;
      requestScope = 0;
    }
    httpcookies.clear();
    encodingRead = false;
    username.clear();
    password.clear();

    releaseLocks();

    if (applicationScope)
    {
      if (applicationScope->release() == 0)
        delete applicationScope;
      applicationScope = 0;
    }

    if (sessionScope)
    {
      if (sessionScope->release() == 0)
        delete sessionScope;
      sessionScope = 0;
    }

    if (secureSessionScope)
    {
      if (secureSessionScope->release() == 0)
        delete secureSessionScope;
      secureSessionScope = 0;
    }

    threadContext = 0;
  }

  void HttpRequest::setMethod(const char* m)
  {
    if (strlen(m) >= 7)
      throw HttpError(HTTP_BAD_REQUEST, "invalid method");
    std::strcpy(method, m);
  }

  std::string HttpRequest::getArgDef(args_type::size_type n, const std::string& def) const
  {
    std::ostringstream k;
    k << "arg" << n;
    return getArg(k.str(), def);
  }

  std::string HttpRequest::getArg(const std::string& name, const std::string& def) const
  {
    args_type::const_iterator it = args.find(name);
    return it == args.end() ? def : it->second;
  }

  void HttpRequest::parse(std::istream& in)
  {
    Parser p(*this);
    p.parse(in);
    if (!p.failed())
      doPostParse();
  }

  void HttpRequest::doPostParse()
  {
    if (hasHeader("Expect:"))
      throw HttpError(HTTP_EXPECTATION_FAILED, "expectation failed", "Expect not supported by this server");

    getparam.parse_url(getQueryString());

    if (isMethodPOST())
    {
      std::istringstream in(getHeader(httpheader::contentType));
      in >> ct;

      if (in)
      {
        if (ct.isMultipart())
        {
          mp.set(ct.getBoundary(), getBody());
          for (Multipart::const_iterator it = mp.begin();
               it != mp.end(); ++it)
          {
            // don't copy uploaded files into qparam to prevent unnecessery
            // copies of large chunks
            if (it->getFilename().empty())
            {
              std::string multipartBody(it->getBodyBegin(), it->getBodyEnd());
              postparam.add(it->getName(), multipartBody);
            }
          }
        }
        else if (ct.getType() == "application"
              && ct.getSubtype() == "x-www-form-urlencoded")
        {
          postparam.parse_url(getBody());
        }
      }
    }

    qparam.add(getparam);
    qparam.add(postparam);

    serial = cxxtools::atomicIncrement(serial_);
  }

  namespace
  {
#ifdef ENABLE_LOCALE
    typedef std::map<std::string, std::locale> locale_map_type;
    static const std::locale* stdlocalePtr = 0;
    static const std::locale* stdlocale = 0;
    static locale_map_type locale_map;
    static cxxtools::Mutex locale_monitor;

    const std::locale& getCacheLocale(const std::string& lang)
    {
      if (stdlocale == 0)
      {
        cxxtools::MutexLock lock(locale_monitor);
        if (stdlocale == 0)
        {
          try
          {
            stdlocalePtr = new std::locale("");
            stdlocale = stdlocalePtr;
          }
          catch (const std::exception& e)
          {
            log_warn("error initializing standard-locale - using locale \"C\"");
            stdlocale = &std::locale::classic();
          }
        }
      }

      if (lang.empty() || lang == stdlocale->name())
        return *stdlocale;

      try
      {
        cxxtools::MutexLock lock(locale_monitor);
        locale_map_type::const_iterator it = locale_map.find(lang);
        if (it == locale_map.end())
        {
          std::locale loc = std::locale(lang.c_str());
          return locale_map.insert(locale_map_type::value_type(lang, loc)).first->second;
        }
        else
          return it->second;
      }
      catch (const std::exception& e)
      {
        log_warn("unknown locale " << lang << ": " << e.what());
        locale_map.insert(locale_map_type::value_type(lang, *stdlocale));
        return *stdlocale;
      }
    }

    void clearLocaleCache()
    {
      cxxtools::MutexLock lock(locale_monitor);
      locale_map.clear();
      delete stdlocalePtr;
      stdlocalePtr = 0;
      stdlocale = 0;
    }
#endif
  }

  const std::locale& HttpRequest::getLocale() const
  {
#ifdef ENABLE_LOCALE
    if (!locale_init)
    {
      static const std::string LANG = "LANG";
      lang = qparam[LANG];
      locale = getCacheLocale(qparam[LANG]);
      if (lang.empty())
        lang = locale.name();
      locale_init = true;
    }

    return locale;
#else
    return std::locale::classic();
#endif
  }

  void HttpRequest::setLocale(const std::locale& loc)
  {
#ifdef ENABLE_LOCALE
    locale_init = true;
    locale = loc;
    lang = loc.name();
#endif
  }

  void HttpRequest::setLang(const std::string& lang_)
  {
#ifdef ENABLE_LOCALE
    lang = lang_;
    locale = getCacheLocale(lang_);
    locale_init = true;
#endif
  }

  const Cookies& HttpRequest::getCookies() const
  {
    if (!httpcookies.hasCookies())
    {
      header_type::const_iterator it = header.find(httpheader::cookie);
      if (it != header.end())
        const_cast<HttpRequest*>(this)->httpcookies.set(it->second);
    }

    return httpcookies;
  }

  const Encoding& HttpRequest::getEncoding() const
  {
    if (!encodingRead)
    {
      encoding.parse(getHeader(httpheader::acceptEncoding));
      encodingRead = true;
    }
    return encoding;
  }

  const std::string& HttpRequest::getUsername() const
  {
    if (username.empty() && hasHeader(httpheader::authorization))
    {
      std::istringstream authHeader(getHeader(httpheader::authorization));
      while (authHeader && authHeader.get() != ' ')
        ;
      cxxtools::Base64istream in(authHeader);
      std::getline(in, username, ':');
      std::getline(in, password);
    }

    return username;
  }

  const std::string& HttpRequest::getPassword() const
  {
    getUsername();
    return password;
  }

  bool HttpRequest::verifyPassword(const std::string& password_) const
  {
    getUsername();
    return password == password_;
  }

  bool HttpRequest::keepAlive() const
  {
    // request is keep alive when either a connection header is explicitely set to keep alive
    // or the http version is 1.1
    Messageheader::const_iterator it = header.find(httpheader::connection);

    return it == header.end()
               ? getMinorVersion() >= 1 && getMajorVersion() >= 1
               : tnt::StringCompareIgnoreCase<const char*>(it->second,
                    httpheader::connectionKeepAlive) == 0;
  }

  const Contenttype& HttpRequest::getContentTypePriv() const
  {
    std::istringstream in(getHeader(httpheader::contentType));
    in >> ct;
    return ct;
  }

  void HttpRequest::setApplicationScope(Scope* s)
  {
    if (applicationScope == s)
      return;

    if (applicationScope)
    {
      releaseApplicationScopeLock();
      if (applicationScope->release() == 0)
        delete applicationScope;
    }

    if (s)
      s->addRef();
    applicationScope = s;
  }

  void HttpRequest::setSessionScope(Sessionscope* s)
  {
    if (sessionScope == s)
      return;

    if (sessionScope)
    {
      if (sessionScopeLocked)
      {
        sessionScope->unlock();
        sessionScopeLocked = false;
      }
      if (sessionScope->release() == 0)
        delete sessionScope;
    }

    if (s)
      s->addRef();

    sessionScope = s;
  }

  void HttpRequest::setSecureSessionScope(Sessionscope* s)
  {
    if (secureSessionScope == s)
      return;

    if (secureSessionScope)
    {
      if (secureSessionScopeLocked)
      {
        secureSessionScope->unlock();
        secureSessionScopeLocked = false;
      }
      if (secureSessionScope->release() == 0)
        delete secureSessionScope;
    }

    if (s)
      s->addRef();

    secureSessionScope = s;
  }

  void HttpRequest::clearSession()
  {
    log_info("end session");

    if (sessionScope)
    {
      if (sessionScopeLocked)
      {
        sessionScope->unlock();
        sessionScopeLocked = false;
      }

      if (sessionScope->release() == 0)
        delete sessionScope;
      sessionScope = 0;
    }

    if (secureSessionScope)
    {
      if (secureSessionScopeLocked)
      {
        secureSessionScope->unlock();
        secureSessionScopeLocked = false;
      }

      if (secureSessionScope->release() == 0)
        delete secureSessionScope;
      secureSessionScope = 0;
    }

  }

  void HttpRequest::ensureApplicationScopeLock()
  {
    ensureSessionScopeLock();
    if (applicationScope && !applicationScopeLocked)
    {
      applicationScope->lock();
      applicationScopeLocked = true;
    }
  }

  void HttpRequest::ensureSessionScopeLock()
  {
    if (sessionScope && !sessionScopeLocked)
    {
      sessionScope->lock();
      sessionScopeLocked = true;
    }

    if (secureSessionScope && !secureSessionScopeLocked)
    {
      secureSessionScope->lock();
      secureSessionScopeLocked = true;
    }

  }

  void HttpRequest::releaseApplicationScopeLock()
  {
    releaseSessionScopeLock();

    if (applicationScope && applicationScopeLocked)
    {
      applicationScopeLocked = false;
      applicationScope->unlock();
    }
  }

  void HttpRequest::releaseSessionScopeLock()
  {
    if (secureSessionScope && secureSessionScopeLocked)
    {
      secureSessionScopeLocked = false;
      secureSessionScope->unlock();
    }

    if (sessionScope && sessionScopeLocked)
    {
      sessionScopeLocked = false;
      sessionScope->unlock();
    }
  }

  Scope& HttpRequest::getRequestScope()
  {
    if (requestScope == 0)
      requestScope = new Scope();
    return *requestScope;
  }

  Scope& HttpRequest::getThreadScope()
  {
    if (threadContext == 0)
      throwRuntimeError("threadcontext not set");
    return threadContext->getScope();
  }

  Scope& HttpRequest::getApplicationScope()
  {
    ensureApplicationScopeLock();
    return *applicationScope;
  }

  Sessionscope& HttpRequest::getSessionScope()
  {
    if (!sessionScope)
      sessionScope = new Sessionscope();
    ensureSessionScopeLock();
    return *sessionScope;
  }

  Sessionscope& HttpRequest::getSecureSessionScope()
  {
    if (!secureSessionScope)
      secureSessionScope = new Sessionscope();
    ensureSessionScopeLock();
    return *secureSessionScope;
  }

  bool HttpRequest::hasSessionScope() const
  {
    return sessionScope != 0 && !sessionScope->empty();
  }

  bool HttpRequest::hasSecureSessionScope() const
  {
    return secureSessionScope != 0 && !secureSessionScope->empty();
  }

  void HttpRequest::postRunCleanup()
  {
#ifdef ENABLE_LOCALE
    tnt::clearLocaleCache();
#endif
  }
}

