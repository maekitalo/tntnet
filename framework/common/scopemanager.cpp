/*
 * Copyright (C) 2003-2006 Tommi Maekitalo
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


#include "tnt/scopemanager.h"
#include "tnt/sessionscope.h"
#include "tnt/httprequest.h"
#include "tnt/httpreply.h"
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>
#include <pthread.h>
#include <stdlib.h>

log_define("tntnet.scopemanager")

namespace tnt
{
  ScopeManager::ScopeManager()
  {
  }

  ScopeManager::~ScopeManager()
  {
    for (sessionscopes_type::iterator it = _sessionScopes.begin(); it != _sessionScopes.end(); ++it)
      delete it->second;
    for (scopes_type::iterator it = _applicationScopes.begin(); it != _applicationScopes.end(); ++it)
      delete it->second;
  }

  Scope* ScopeManager::getApplicationScope(const std::string& appname)
  {
    cxxtools::MutexLock lock(_applicationScopesMutex);

    scopes_type::iterator it = _applicationScopes.find(appname);
    if (it == _applicationScopes.end())
    {
      log_debug("applicationscope not found - create new");
      Scope* s = new Scope();
      it = _applicationScopes.insert(scopes_type::value_type(appname, s)).first;
      return s;
    }
    else
      log_debug("applicationscope found");

    return it->second;
  }

  Sessionscope* ScopeManager::getSessionScope(const std::string& sessioncookie)
  {
    log_debug("getSessionScope(\"" << sessioncookie << "\")");

    cxxtools::MutexLock lock(_sessionScopesMutex);
    sessionscopes_type::iterator it = _sessionScopes.find(sessioncookie);
    if (it == _sessionScopes.end())
    {
      log_debug("session " << sessioncookie << " not found");
      return 0;
    }
    else
    {
      log_debug("session " << sessioncookie << " found");
      it->second->touch();
      return it->second;
    }
  }

  bool ScopeManager::hasSessionScope(const std::string& sessioncookie)
  {
    cxxtools::MutexLock lock(_sessionScopesMutex);
    sessionscopes_type::iterator it = _sessionScopes.find(sessioncookie);
    return it != _sessionScopes.end();
  }

  void ScopeManager::putSessionScope(const std::string& sessioncookie, Sessionscope* s)
  {
    s->addRef();

    cxxtools::MutexLock lock(_sessionScopesMutex);
    sessionscopes_type::iterator it = _sessionScopes.find(sessioncookie);
    if (it != _sessionScopes.end())
    {
      if (it->second->release() == 0)
        delete it->second;
      it->second = s;
    }
    else
      _sessionScopes[sessioncookie] = s;
  }


  void ScopeManager::removeApplicationScope(const std::string& appname)
  {
    cxxtools::MutexLock lock(_applicationScopesMutex);
    scopes_type::iterator it = _applicationScopes.find(appname);
    if (it != _applicationScopes.end())
    {
      if (it->second->release() == 0)
        delete it->second;
      _applicationScopes.erase(it);
    }
  }

  void ScopeManager::removeSessionScope(const std::string& sessioncookie)
  {
    cxxtools::MutexLock lock(_sessionScopesMutex);
    sessionscopes_type::iterator it = _sessionScopes.find(sessioncookie);
    if (it != _sessionScopes.end())
    {
      if (it->second->release() == 0)
        delete it->second;
      _sessionScopes.erase(it);
    }
  }

  void ScopeManager::preCall(HttpRequest& request, const std::string& app)
  {
    // check session-cookie
    std::string currentSessionCookieName = app.empty() ? std::string("tntnet") : "tntnet." + app;
    std::string currentSecureSessionCookieName = app.empty() ? std::string("stntnet") : "stntnet." + app;

    Cookie c = request.getCookie(currentSessionCookieName);
    if (c.getValue().empty())
    {
      /*
      cxxtools::MutexLock lock(sessionScopesMutex);
      log_debug(sessionScopes.size() << " sessions available");
      for (sessionscopes_type::iterator it = sessionScopes.begin(); it != sessionScopes.end(); ++it)
        log_debug("available session " << it->first << " value " << it->second);
        */

      log_debug("session cookie " << currentSessionCookieName << " not found - keep session");
    }
    else
    {
      log_debug("session cookie " << currentSessionCookieName << " found: " << c.getValue());

      cxxtools::MutexLock lock(_sessionScopesMutex);

      Sessionscope* sessionScope;

      sessionscopes_type::iterator it = _sessionScopes.find(c.getValue());
      if (it == _sessionScopes.end())
      {
        log_debug("session not found - create new");
        sessionScope = new Sessionscope();
        sessionScope->addRef();
        _sessionScopes.insert(sessionscopes_type::value_type(c.getValue(), sessionScope));
      }
      else
      {
        log_debug("session found");
        sessionScope = it->second;
        sessionScope->touch();
      }

      request.setSessionScope(sessionScope);
    }

    if (request.isSsl())
    {
      c = request.getCookie(currentSecureSessionCookieName);
      if (c.getValue().empty())
      {
        log_debug("secure session cookie " << currentSessionCookieName
            << " not found - keep session");
      }
      else if (request.isSsl())
      {
        log_debug("secure session cookie " << currentSessionCookieName
            << " found: " << c.getValue());

        cxxtools::MutexLock lock(_sessionScopesMutex);

        Sessionscope* sessionScope;

        sessionscopes_type::iterator it = _sessionScopes.find(c.getValue());
        if (it == _sessionScopes.end())
        {
          log_debug("session not found - create new");
          sessionScope = new Sessionscope();
          sessionScope->addRef();
          _sessionScopes.insert(sessionscopes_type::value_type(c.getValue(), sessionScope));
        }
        else
        {
          log_debug("session found");
          sessionScope = it->second;
          sessionScope->touch();
        }

        request.setSecureSessionScope(sessionScope);
      }
    }
    else
    {
      log_debug("secure session cookie " << currentSessionCookieName
          << " not checked in non ssl request");
    }

    // set application-scope
    request.setApplicationScope(getApplicationScope(app));
  }

  void ScopeManager::setSessionId(HttpRequest& request, const std::string& sessionId)
  {
    if (sessionId.empty())
    {
      request.setSessionScope(0);
    }
    else
    {
      Sessionscope* sessionScope = getSessionScope(sessionId);
      if (sessionScope != 0)
      {
        log_debug("session found");
        request.setSessionScope(sessionScope);
      }
    }
  }

  std::string ScopeManager::postCall(HttpRequest& request, HttpReply& reply, const std::string& app)
  {
    std::string currentSessionCookieName = app.empty() ? std::string("tntnet") : "tntnet." + app;
    std::string currentSecureSessionCookieName = app.empty() ? std::string("stntnet") : "stntnet." + app;

    std::string sessionId;

    if (reply.isClearSession())
    {
      sessionId = request.getCookie(currentSessionCookieName);
      if (!sessionId.empty())
        removeSessionScope(sessionId);

      std::string secureSessionId = request.getCookie(currentSecureSessionCookieName);
      if (!secureSessionId.empty())
        removeSessionScope(secureSessionId);
    }
    else
    {
      if (request.hasSessionScope())
      {
        // request has session scope
        sessionId = request.getCookie(currentSessionCookieName);
        if (sessionId.empty())
        {
          // client has no sessionId

          cxxtools::Md5stream c;
          c << request.getSerial() << '-' << ::pthread_self() << '-' << rand();
          sessionId = c.getHexDigest();
          log_info("create new session " << sessionId);
          reply.setCookie(currentSessionCookieName, sessionId);
          putSessionScope(sessionId, &request.getSessionScope());
        }
        else if (!hasSessionScope(sessionId))
        {
          // client has a sessionId but no associated session
          // this may happen, when tntnet is restarted while the browser has a session

          putSessionScope(sessionId, &request.getSessionScope());
        }
      }

      if (request.isSsl() && request.hasSecureSessionScope())
      {
        // request has secure session scope
        std::string sessionId = request.getCookie(currentSecureSessionCookieName);
        if (sessionId.empty())
        {
          // client has no sessionId

          cxxtools::Md5stream c;
          c << request.getSerial() << '-' << ::pthread_self() << '-' << rand();
          sessionId = c.getHexDigest();
          log_info("create new secure session " << sessionId);
          tnt::Cookie cookie(sessionId);
          cookie.setSecure();
          reply.setCookie(currentSecureSessionCookieName, cookie);
          putSessionScope(sessionId, &request.getSecureSessionScope());
        }
        else if (!hasSessionScope(sessionId))
        {
          // client has a sessionId but no associated session
          // this may happen, when tntnet is restarted while the browser has a session

          putSessionScope(sessionId, &request.getSecureSessionScope());
        }
      }
    }

    return sessionId;
  }

  void ScopeManager::checkSessionTimeout()
  {
    time_t currentTime;
    time(&currentTime);
    cxxtools::MutexLock lock(_sessionScopesMutex);
    sessionscopes_type::iterator it = _sessionScopes.begin();
    unsigned count = 0;
    while (it != _sessionScopes.end())
    {
      Sessionscope* s = it->second;
      if (static_cast <unsigned> (currentTime - s->getAtime()) > s->getTimeout())
      {
        log_info("sessiontimeout for session " << it->first << " reached");
        sessionscopes_type::iterator it2 = it;
        ++it;
        if (s->release() == 0)
          delete s;
        _sessionScopes.erase(it2);
        ++count;
      }
      else
        ++it;
    }

  }
}

