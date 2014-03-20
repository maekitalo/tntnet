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
  cxxtools::atomic_t HttpRequest::_nextSerial = 0;

  HttpRequest::HttpRequest(Tntnet& application, const SocketIf* socketIf)
    : _socketIf(socketIf),
      _localeInit(false),
      _encodingRead(false),
      _requestScope(0),
      _applicationScope(0),
      _sessionScope(0),
      _secureSessionScope(0),
      _threadContext(0),
      _applicationScopeLocked(false),
      _sessionScopeLocked(false),
      _secureSessionScopeLocked(false),
      _application(application)
  {
  }

  HttpRequest::HttpRequest(Tntnet& application, const std::string& url, const SocketIf* socketIf)
    : _socketIf(socketIf),
      _localeInit(false),
      _requestScope(0),
      _applicationScope(0),
      _sessionScope(0),
      _secureSessionScope(0),
      _threadContext(0),
      _applicationScopeLocked(false),
      _sessionScopeLocked(false),
      _secureSessionScopeLocked(false),
      _application(application)
  {
    std::istringstream s("GET " + url + " HTTP/1.1\r\n\r\n");
    parse(s);
  }

  HttpRequest::HttpRequest(const HttpRequest& r)
    : _methodLen(0),
      _pathinfo(r._pathinfo),
      _args(r._args),
      _getparam(r._getparam),
      _postparam(r._postparam),
      _qparam(r._qparam),
      _socketIf(r._socketIf),
      _ct(r._ct),
      _mp(r._mp),
      _serial(r._serial),
      _localeInit(r._localeInit),
      _requestScope(r._requestScope),
      _applicationScope(r._applicationScope),
      _sessionScope(r._sessionScope),
      _secureSessionScope(r._secureSessionScope),
      _threadContext(r._threadContext),
      _applicationScopeLocked(false),
      _sessionScopeLocked(false),
      _secureSessionScopeLocked(false),
      _application(r._application)
  {
    if (_requestScope)
      _requestScope->addRef();
    if (_applicationScope)
      _applicationScope->addRef();
    if (_sessionScope)
      _sessionScope->addRef();
    if (_secureSessionScope)
      _secureSessionScope->addRef();
  }

  HttpRequest::~HttpRequest()
  {
    releaseLocks();

    if (_requestScope && _requestScope->release() == 0)
      delete _requestScope;
    if (_applicationScope && _applicationScope->release() == 0)
      delete _applicationScope;
    if (_sessionScope && _sessionScope->release() == 0)
      delete _sessionScope;
    if (_secureSessionScope && _secureSessionScope->release() == 0)
      delete _secureSessionScope;
  }

  HttpRequest& HttpRequest::operator= (const HttpRequest& r)
  {
    _pathinfo = r._pathinfo;
    _args = r._args;
    _getparam = r._getparam;
    _postparam = r._postparam;
    _qparam = r._qparam;
    _ct = r._ct;
    _mp = r._mp;
    _socketIf = r._socketIf;
    _serial = r._serial;
    _localeInit = r._localeInit;
    _requestScope = r._requestScope;
    _applicationScope = r._applicationScope;
    _sessionScope = r._sessionScope;
    _secureSessionScope = r._secureSessionScope;
    _threadContext = r._threadContext;
    _applicationScopeLocked = false;
    _sessionScopeLocked = false;
    _secureSessionScopeLocked = false;

    if (_requestScope)
      _requestScope->addRef();
    if (_applicationScope)
      _applicationScope->addRef();
    if (_sessionScope)
      _sessionScope->addRef();
    if (_secureSessionScope)
      _secureSessionScope->addRef();

    return *this;
  }

  void HttpRequest::clear()
  {
    HttpMessage::clear();
    _body.clear();
    _methodLen = 0;
    _method[0] = '\0';
    _url.clear();
    _queryString.clear();
    _contentSize = 0;
    _pathinfo.clear();
    _args.clear();
    _getparam.clear();
    _postparam.clear();
    _qparam.clear();
    _ct = Contenttype();
    _mp = Multipart();
    _localeInit = false;
    if (_requestScope)
    {
      if (_requestScope->release() == 0)
        delete _requestScope;
      _requestScope = 0;
    }
    httpcookies.clear();
    _encodingRead = false;
    _username.clear();
    _password.clear();

    releaseLocks();

    if (_applicationScope)
    {
      if (_applicationScope->release() == 0)
        delete _applicationScope;
      _applicationScope = 0;
    }

    if (_sessionScope)
    {
      if (_sessionScope->release() == 0)
        delete _sessionScope;
      _sessionScope = 0;
    }

    if (_secureSessionScope)
    {
      if (_secureSessionScope->release() == 0)
        delete _secureSessionScope;
      _secureSessionScope = 0;
    }

    _threadContext = 0;
  }

  void HttpRequest::setMethod(const char* m)
  {
    if (strlen(m) >= 7)
      throw HttpError(HTTP_BAD_REQUEST, "invalid method");
    std::strcpy(_method, m);
  }

  std::string HttpRequest::getArgDef(args_type::size_type n, const std::string& def) const
  {
    std::ostringstream k;
    k << "arg" << n;
    return getArg(k.str(), def);
  }

  std::string HttpRequest::getArg(const std::string& name, const std::string& def) const
  {
    args_type::const_iterator it = _args.find(name);
    return it == _args.end() ? def : it->second;
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

    _getparam.parse_url(getQueryString());

    if (isMethodPOST())
    {
      std::istringstream in(getHeader(httpheader::contentType));
      in >> _ct;

      if (in)
      {
        if (_ct.isMultipart())
        {
          _mp.set(_ct.getBoundary(), getBody());
          for (Multipart::const_iterator it = _mp.begin(); it != _mp.end(); ++it)
          {
            // don't copy uploaded files into qparam to prevent unnecessery
            // copies of large chunks
            if (it->getFilename().empty())
            {
              std::string multipartBody(it->getBodyBegin(), it->getBodyEnd());
              _postparam.add(it->getName(), multipartBody);
            }
          }
        }
        else if (_ct.getType() == "application"
              && _ct.getSubtype() == "x-www-form-urlencoded")
        {
          _postparam.parse_url(getBody());
        }
      }
    }

    _qparam.add(_getparam);
    _qparam.add(_postparam);

    _serial = cxxtools::atomicIncrement(_nextSerial);
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
    if (!_localeInit)
    {
      static const std::string LANG = "LANG";
      _lang = _qparam[LANG];
      const_cast<QueryParams&>(_qparam).locale(getCacheLocale(_qparam[LANG]));
      if (_lang.empty())
        _lang = _qparam.locale().name();
      _localeInit = true;
    }

    return _qparam.locale();
#else
    return std::locale::classic();
#endif
  }

  void HttpRequest::setLocale(const std::locale& loc)
  {
#ifdef ENABLE_LOCALE
    _localeInit = true;
    _qparam.locale(loc);
    _lang = loc.name();
#endif
  }

  void HttpRequest::setLang(const std::string& lang)
  {
#ifdef ENABLE_LOCALE
    _lang = lang;
    _qparam.locale(getCacheLocale(lang));
    _localeInit = true;
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
    if (!_encodingRead)
    {
      _encoding.parse(getHeader(httpheader::acceptEncoding));
      _encodingRead = true;
    }
    return _encoding;
  }

  const std::string& HttpRequest::getUsername() const
  {
    if (_username.empty() && hasHeader(httpheader::authorization))
    {
      std::istringstream authHeader(getHeader(httpheader::authorization));
      while (authHeader && authHeader.get() != ' ')
        ;
      cxxtools::Base64istream in(authHeader);
      std::getline(in, _username, ':');
      std::getline(in, _password);
    }

    return _username;
  }

  const std::string& HttpRequest::getPassword() const
  {
    getUsername();
    return _password;
  }

  bool HttpRequest::verifyPassword(const std::string& password) const
  {
    getUsername();
    return _password == password;
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
    in >> _ct;
    return _ct;
  }

  void HttpRequest::setApplicationScope(Scope* s)
  {
    if (_applicationScope == s)
      return;

    if (_applicationScope)
    {
      releaseApplicationScopeLock();
      if (_applicationScope->release() == 0)
        delete _applicationScope;
    }

    if (s)
      s->addRef();
    _applicationScope = s;
  }

  void HttpRequest::setSessionScope(Sessionscope* s)
  {
    if (_sessionScope == s)
      return;

    if (_sessionScope)
    {
      if (_sessionScopeLocked)
      {
        _sessionScope->unlock();
        _sessionScopeLocked = false;
      }
      if (_sessionScope->release() == 0)
        delete _sessionScope;
    }

    if (s)
      s->addRef();

    _sessionScope = s;
  }

  void HttpRequest::setSecureSessionScope(Sessionscope* s)
  {
    if (_secureSessionScope == s)
      return;

    if (_secureSessionScope)
    {
      if (_secureSessionScopeLocked)
      {
        _secureSessionScope->unlock();
        _secureSessionScopeLocked = false;
      }
      if (_secureSessionScope->release() == 0)
        delete _secureSessionScope;
    }

    if (s)
      s->addRef();

    _secureSessionScope = s;
  }

  void HttpRequest::ensureApplicationScopeLock()
  {
    ensureSessionScopeLock();
    if (_applicationScope && !_applicationScopeLocked)
    {
      _applicationScope->lock();
      _applicationScopeLocked = true;
    }
  }

  void HttpRequest::ensureSessionScopeLock()
  {
    if (_sessionScope && !_sessionScopeLocked)
    {
      _sessionScope->lock();
      _sessionScopeLocked = true;
    }

    if (_secureSessionScope && !_secureSessionScopeLocked)
    {
      _secureSessionScope->lock();
      _secureSessionScopeLocked = true;
    }
  }

  void HttpRequest::releaseApplicationScopeLock()
  {
    releaseSessionScopeLock();

    if (_applicationScope && _applicationScopeLocked)
    {
      _applicationScopeLocked = false;
      _applicationScope->unlock();
    }
  }

  void HttpRequest::releaseSessionScopeLock()
  {
    if (_secureSessionScope && _secureSessionScopeLocked)
    {
      _secureSessionScopeLocked = false;
      _secureSessionScope->unlock();
    }

    if (_sessionScope && _sessionScopeLocked)
    {
      _sessionScopeLocked = false;
      _sessionScope->unlock();
    }
  }

  Scope& HttpRequest::getRequestScope()
  {
    if (_requestScope == 0)
      _requestScope = new Scope();
    return *_requestScope;
  }

  Scope& HttpRequest::getThreadScope()
  {
    if (_threadContext == 0)
      throwRuntimeError("threadcontext not set");
    return _threadContext->getScope();
  }

  Scope& HttpRequest::getApplicationScope()
  {
    ensureApplicationScopeLock();
    return *_applicationScope;
  }

  Sessionscope& HttpRequest::getSessionScope()
  {
    if (!_sessionScope)
      _sessionScope = new Sessionscope();
    ensureSessionScopeLock();
    return *_sessionScope;
  }

  Sessionscope& HttpRequest::getSecureSessionScope()
  {
    if (!_secureSessionScope)
      _secureSessionScope = new Sessionscope();
    ensureSessionScopeLock();
    return *_secureSessionScope;
  }

  bool HttpRequest::hasSessionScope() const
  {
    return _sessionScope != 0 && !_sessionScope->empty();
  }

  bool HttpRequest::hasSecureSessionScope() const
  {
    return _secureSessionScope != 0 && !_secureSessionScope->empty();
  }

  void HttpRequest::postRunCleanup()
  {
#ifdef ENABLE_LOCALE
    tnt::clearLocaleCache();
#endif
  }
}

