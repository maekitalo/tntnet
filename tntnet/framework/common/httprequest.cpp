/* httprequest.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <sstream>
#include <cxxtools/log.h>
#include <cxxtools/thread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tnt/sessionscope.h>
#include <errno.h>
#include <string.h>
#include "config.h"

namespace tnt
{
  log_define("tntnet.httprequest")

  ////////////////////////////////////////////////////////////////////////
  // HttpRequest
  //
  unsigned HttpRequest::serial_ = 0;

  HttpRequest::HttpRequest(const std::string& url)
    : ssl(false),
      locale_init(false),
      requestScope(0),
      applicationScope(0),
      threadScope(0),
      sessionScope(0),
      applicationScopeLocked(false),
      sessionScopeLocked(false)
  {
    std::istringstream s("GET " + url + " HTTP/1.1\r\n\r\n");
    parse(s);
  }

  HttpRequest::HttpRequest(const HttpRequest& r)
    : pathinfo(r.pathinfo),
      args(r.args),
      qparam(r.qparam),
      peerAddr(r.peerAddr),
      serverAddr(r.serverAddr),
      ct(r.ct),
      mp(r.mp),
      ssl(r.ssl),
      serial(r.serial),
      locale_init(r.locale_init),
      locale(r.locale),
      requestScope(r.requestScope),
      applicationScope(r.applicationScope),
      threadScope(r.threadScope),
      sessionScope(r.sessionScope),
      applicationScopeLocked(false),
      sessionScopeLocked(false)
  {
    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (threadScope)
      threadScope->addRef();
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
    if (threadScope)
      threadScope->release();
    if (sessionScope)
      sessionScope->release();
  }

  HttpRequest& HttpRequest::operator= (const HttpRequest& r)
  {
    pathinfo = r.pathinfo;
    args = r.args;
    qparam = r.qparam;
    peerAddr = r.peerAddr;
    serverAddr = r.serverAddr;
    ct = r.ct;
    mp = r.mp;
    ssl = r.ssl;
    serial = r.serial;
    locale_init = r.locale_init;
    locale = r.locale;
    requestScope = r.requestScope;
    applicationScope = r.applicationScope;
    threadScope = r.threadScope;
    sessionScope = r.sessionScope;
    applicationScopeLocked = false;
    sessionScopeLocked = false;

    if (requestScope)
      requestScope->addRef();
    if (applicationScope)
      applicationScope->addRef();
    if (threadScope)
      threadScope->addRef();
    if (sessionScope)
      sessionScope->addRef();

    return *this;
  }

  void HttpRequest::clear()
  {
    HttpMessage::clear();
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

    releaseLocks();

    if (applicationScope)
    {
      applicationScope->release();
      applicationScope = 0;
    }

    if (threadScope)
    {
      threadScope->release();
      threadScope = 0;
    }

    if (sessionScope)
    {
      sessionScope->release();
      sessionScope = 0;
    }
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
        else
        {
          qparam.parse_url(getBody());
        }
      }
      else if (ct.getType() == "application"
            && ct.getSubtype() == "x-www-form-urlencoded")
        qparam.parse_url(getBody());
    }

    {
      static cxxtools::Mutex monitor;
      cxxtools::MutexLock lock(monitor);
      serial = ++serial_;
    }
  }

  namespace
  {
    std::string formatIp(const sockaddr_storage& addr)
    {
#ifdef HAVE_INET_NTOP
      const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
      char strbuf[INET6_ADDRSTRLEN];
      inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
      return strbuf;
#else
      static cxxtools::Mutex monitor;
      cxxtools::MutexLock lock(monitor);

      char* p = inet_ntoa(peerAddr.sin_addr);
      return std::string(p);
#endif
    }
  }

  std::string HttpRequest::getPeerIp() const
  {
    return formatIp(peerAddr);
  }

  std::string HttpRequest::getServerIp() const
  {
    return formatIp(serverAddr);
  }

  const std::locale& HttpRequest::getLocale() const
  {
    if (!locale_init)
    {
      static const std::string LANG = "LANG";
      static const std::locale stdlocale("");
      typedef std::map<std::string, std::locale> locale_map_type;
      static locale_map_type locale_map;
      static cxxtools::Mutex locale_monitor;

      locale_init = true;

      log_debug("HttpRequest::getLocale() " << qparam.dump());

      std::string lang = qparam[LANG];

      if (lang.empty() || lang == stdlocale.name())
        locale = stdlocale;
      else
      {
        log_debug("LANG from query-parameter (" << lang << ')');
        try
        {
          cxxtools::MutexLock lock(locale_monitor);
          locale_map_type::const_iterator it = locale_map.find(lang);
          if (it == locale_map.end())
          {
            locale = std::locale(lang.c_str());
            locale_map.insert(locale_map_type::value_type(lang, locale));
          }
          else
            locale = it->second;
        }
        catch (const std::exception& e)
        {
          log_warn("unknown locale " << lang << ": " << e.what());
          locale = stdlocale;
          locale_map.insert(locale_map_type::value_type(lang, locale));
        }
      }

      log_debug("LANG=" << locale.name());
    }

    return locale;
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

  void HttpRequest::setApplicationScope(Scope* s)
  {
    if (applicationScope)
    {
      releaseApplicationScopeLock();
      applicationScope->release();
    }

    if (s)
      s->addRef();
    applicationScope = s;
  }

  void HttpRequest::setThreadScope(Scope* s)
  {
    if (threadScope)
      threadScope->release();

    if (s)
      s->addRef();

    threadScope = s;
  }

  void HttpRequest::setSessionScope(Sessionscope* s)
  {
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

