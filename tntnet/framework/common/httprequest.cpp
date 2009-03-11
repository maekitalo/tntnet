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

namespace tnt
{
  log_define("tntnet.httprequest")

  ////////////////////////////////////////////////////////////////////////
  // HttpRequest
  //
  size_t HttpRequest::maxRequestSize = 0;
  cxxtools::atomic_t HttpRequest::serial_ = 0;

  HttpRequest::HttpRequest(Tntnet& application_, const SocketIf* socketIf_)
    : socketIf(socketIf_),
      locale_init(false),
      encodingRead(false),
      requestScope(0),
      applicationScope(0),
      sessionScope(0),
      threadContext(0),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      application(application_)
  {
  }

  HttpRequest::HttpRequest(Tntnet& application_, const std::string& url, const SocketIf* socketIf_)
    : socketIf(socketIf_),
      locale_init(false),
      requestScope(0),
      applicationScope(0),
      sessionScope(0),
      threadContext(0),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      application(application_)
  {
    std::istringstream s("GET " + url + " HTTP/1.1\r\n\r\n");
    parse(s);
  }

  HttpRequest::HttpRequest(const HttpRequest& r)
    : pathinfo(r.pathinfo),
      args(r.args),
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
      threadContext(r.threadContext),
      applicationScopeLocked(false),
      sessionScopeLocked(false),
      application(r.application)
  {
    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (sessionScope)
      sessionScope->addRef();
  }

  HttpRequest::~HttpRequest()
  {
    releaseLocks();

    if (requestScope)
      requestScope->release();
    if (applicationScope)
      applicationScope->release();
    if (sessionScope)
      sessionScope->release();
  }

  HttpRequest& HttpRequest::operator= (const HttpRequest& r)
  {
    pathinfo = r.pathinfo;
    args = r.args;
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
    threadContext = r.threadContext;
    applicationScopeLocked = false;
    sessionScopeLocked = false;

    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (sessionScope)
      sessionScope->addRef();

    return *this;
  }

  void HttpRequest::clear()
  {
    HttpMessage::clear();
    body.clear();
    method.clear();
    url.clear();
    queryString.clear();
    contentSize = 0;
    pathinfo.clear();
    args.clear();
    qparam.clear();
    ct = Contenttype();
    mp = Multipart();
    locale_init = false;
    if (requestScope)
    {
      requestScope->release();
      requestScope = 0;
    }
    httpcookies.clear();
    encodingRead = false;
    username.clear();
    password.clear();

    releaseLocks();

    if (applicationScope)
    {
      applicationScope->release();
      applicationScope = 0;
    }

    if (sessionScope)
    {
      sessionScope->release();
      sessionScope = 0;
    }

    threadContext = 0;
  }

  std::string HttpRequest::getArg(const std::string& name, const std::string& def) const
  {
    for (args_type::const_iterator it = args.begin(); it != args.end(); ++it)
      if (it->size() > name.size()
        && it->compare(0, name.size(), name) == 0
        && it->at(name.size()) == '=')
        return it->substr(name.size() + 1);
    return def;
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
    qparam.parse_url(getQueryString());
    if (getMethod() == "POST")
    {
      std::istringstream in(getHeader(httpheader::contentType));
      in >> ct;

      if (in)
      {
        log_debug(httpheader::contentType << ' ' << in.str());
        log_debug("type=" << ct.getType() << " subtype=" << ct.getSubtype());
        if (ct.isMultipart())
        {
          log_debug("multipart-boundary=" << ct.getBoundary());
          mp.set(ct.getBoundary(), getBody());
          for (Multipart::const_iterator it = mp.begin();
               it != mp.end(); ++it)
          {
            // don't copy uploaded files into qparam to prevent unnecessery
            // copies of large chunks
            if (it->getFilename().empty())
            {
              std::string multipartBody(it->getBodyBegin(), it->getBodyEnd());
              log_debug("multipart-item name=" << it->getName()
                     << " body=" << multipartBody);
              qparam.add(it->getName(), multipartBody);
            }
          }
        }
        else if (ct.getType() == "application"
              && ct.getSubtype() == "x-www-form-urlencoded")
        {
          qparam.parse_url(getBody());
        }
      }
    }

    serial = cxxtools::atomicIncrement(serial_);
  }

  namespace
  {
    const std::locale& getCacheLocale(const std::string& lang)
    {
      static std::locale stdlocale;
      static bool stdlocale_init = false;

      typedef std::map<std::string, std::locale> locale_map_type;
      static locale_map_type locale_map;
      static cxxtools::Mutex locale_monitor;

      if (!stdlocale_init)
      {
        cxxtools::MutexLock lock(locale_monitor);
        if (!stdlocale_init)
        {
          stdlocale_init = true;
          try
          {
            stdlocale = std::locale("");
          }
          catch (const std::exception& e)
          {
            log_warn("error initializing standard-locale - using locale \"C\"");
          }
        }
      }

      if (lang.empty() || lang == stdlocale.name())
        return stdlocale;

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
        locale_map.insert(locale_map_type::value_type(lang, stdlocale));
        return stdlocale;
      }
    }
  }

  const std::locale& HttpRequest::getLocale() const
  {
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
  }

  void HttpRequest::setLocale(const std::locale& loc)
  {
    locale_init = true;
    locale = loc;
    lang = loc.name();
  }

  void HttpRequest::setLang(const std::string& lang_)
  {
    lang = lang_;
    locale = getCacheLocale(lang_);
    locale_init = true;
  }

  const Cookies& HttpRequest::getCookies() const
  {
    log_debug("HttpRequest::getCookies()");

    if (!httpcookies.hasCookies())
    {
      header_type::const_iterator it = header.find(httpheader::cookie);
      if (it != header.end())
      {
        log_debug("parse cookie-header " << it->second);
        const_cast<HttpRequest*>(this)->httpcookies.set(it->second);
      }
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

      log_debug("username \"" << username << "\" password \"" << password << '"');
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
    log_debug("verify password \"" << password_ << "\" for username \"" << username << "\" password \"" << password << '"');
    return password == password_;
  }

  namespace
  {
    class CompareNoCase
    {
      public:
        bool operator() (char c1, char c2) const
          { return tolower(c1) == tolower(c2); }
    };
  }

  bool HttpRequest::keepAlive() const
  {
    header_type::const_iterator it = header.find(httpheader::connection);
    return it == header.end() ? getMajorVersion() == 1 && getMinorVersion() == 1
                              : it->second.size() >= httpheader::connectionKeepAlive.size()
                                && std::equal(it->second.begin(),
                                              it->second.end(),
                                              httpheader::connectionKeepAlive.begin(),
                                              CompareNoCase());
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
      applicationScope->release();
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
      releaseSessionScopeLock();
      sessionScope->release();
    }

    if (s)
      s->addRef();
    sessionScope = s;
  }

  void HttpRequest::clearSession()
  {
    if (sessionScope)
    {
      log_info("end session");
      releaseSessionScopeLock();
      sessionScope->release();
      sessionScope = 0;
    }
  }

  void HttpRequest::ensureApplicationScopeLock()
  {
    log_trace("ensureApplicationScopeLock; thread " << pthread_self());

    ensureSessionScopeLock();
    if (applicationScope && !applicationScopeLocked)
    {
      log_debug("lock application scope; thread" << pthread_self());
      applicationScope->lock();
      applicationScopeLocked = true;
    }
    else
      log_debug("applicationscope locked already");
  }

  void HttpRequest::ensureSessionScopeLock()
  {
    log_trace("ensureSessionScopeLock; thread " << pthread_self());

    if (sessionScope && !sessionScopeLocked)
    {
      log_debug("lock sessionscope; thread " << pthread_self());
      sessionScope->lock();
      sessionScopeLocked = true;
    }
    else
      log_debug("sessionscope locked already");
  }

  void HttpRequest::releaseApplicationScopeLock()
  {
    log_trace("releaseApplicationScopeLock; thread " << pthread_self());

    if (applicationScope && applicationScopeLocked)
    {
      log_debug("unlock applicationscope");
      applicationScopeLocked = false;
      applicationScope->unlock();
    }
    else
      log_debug("applicationscope not locked");
  }

  void HttpRequest::releaseSessionScopeLock()
  {
    log_trace("releaseSessionScopeLock; thread " << pthread_self());

    releaseApplicationScopeLock();

    if (sessionScope && sessionScopeLocked)
    {
      log_debug("unlock sessionscope");
      sessionScopeLocked = false;
      sessionScope->unlock();
    }
    else
      log_debug("sessionscope not locked");
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

  bool HttpRequest::hasSessionScope() const
  {
    return sessionScope != 0 && !sessionScope->empty();
  }
}

