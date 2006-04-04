/* scopemanager.cpp
   Copyright (C) 2003-2006 Tommi Maekitalo

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

#include "tnt/scopemanager.h"
#include "tnt/sessionscope.h"
#include "tnt/httprequest.h"
#include "tnt/httpreply.h"
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>

log_define("tntnet.scopemanager")

namespace tnt
{
  static const std::string& sessionCookiePrefix = "tntnet.";

  Scope* ScopeManager::getApplicationScope(const std::string& appname)
  {
    log_debug("getApplicationScope(\"" << appname << "\")");

    cxxtools::MutexLock lock(applicationScopesMutex);

    scopes_type::iterator it = applicationScopes.find(appname);
    if (it == applicationScopes.end())
    {
      log_debug("applicationscope not found - create new");
      Scope* s = new Scope();
      it = applicationScopes.insert(scopes_type::value_type(appname, s)).first;
      return s;
    }
    else
      log_debug("applicationscope found");

    return it->second;
  }

  Sessionscope* ScopeManager::getSessionScope(const std::string& sessioncookie)
  {
    log_debug("getSessionScope(\"" << sessioncookie << "\")");

    cxxtools::MutexLock lock(sessionScopesMutex);
    sessionscopes_type::iterator it = sessionScopes.find(sessioncookie);
    if (it == sessionScopes.end())
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
    log_debug("hasSessionScope(\"" << sessioncookie << "\")");

    cxxtools::MutexLock lock(sessionScopesMutex);
    sessionscopes_type::iterator it = sessionScopes.find(sessioncookie);
    return it != sessionScopes.end();
  }

  void ScopeManager::putSessionScope(const std::string& sessioncookie, Sessionscope* s)
  {
    log_debug("putSessionScope " << sessioncookie);

    s->addRef();

    cxxtools::MutexLock lock(sessionScopesMutex);
    sessionscopes_type::iterator it = sessionScopes.find(sessioncookie);
    if (it != sessionScopes.end())
    {
      log_debug("sessionscope found - replace");
      it->second->release();
      it->second = s;
    }
    else
      sessionScopes[sessioncookie] = s;
  }


  void ScopeManager::removeApplicationScope(const std::string& appname)
  {
    log_debug("removeApplicationScope(\"" << appname << "\")");

    cxxtools::MutexLock lock(applicationScopesMutex);
    scopes_type::iterator it = applicationScopes.find(appname);
    if (it != applicationScopes.end())
    {
      log_debug("release applicationscope");
      it->second->release();
      applicationScopes.erase(it);
    }
  }

  void ScopeManager::removeSessionScope(const std::string& sessioncookie)
  {
    log_debug("removeSessionScope(\"" << sessioncookie << "\")");

    cxxtools::MutexLock lock(sessionScopesMutex);
    sessionscopes_type::iterator it = sessionScopes.find(sessioncookie);
    if (it != sessionScopes.end())
    {
      log_debug("release sessionscope");
      it->second->release();
      sessionScopes.erase(it);
    }
  }

  void ScopeManager::preCall(HttpRequest& request, const std::string& app)
  {
    // check session-cookie
    std::string currentSessionCookieName = sessionCookiePrefix + app;
    Cookie c = request.getCookie(currentSessionCookieName);
    if (c.getValue().empty())
    {
      log_debug("session-cookie " << currentSessionCookieName << " not found");
      request.setSessionScope(0);
    }
    else
    {
      log_debug("session-cookie " << currentSessionCookieName << " found: " << c.getValue());
      Sessionscope* sessionScope = getSessionScope(c.getValue());
      if (sessionScope != 0)
      {
        log_debug("session found");
        request.setSessionScope(sessionScope);
      }
    }

    // set application-scope
    request.setApplicationScope(getApplicationScope(app));
  }

  void ScopeManager::postCall(HttpRequest& request, HttpReply& reply, const std::string& app)
  {
    std::string currentSessionCookieName = sessionCookiePrefix + app;

    if (request.hasSessionScope())
    {
      // request has session-scope
      std::string cookie = request.getCookie(currentSessionCookieName);
      if (cookie.empty() || !hasSessionScope(cookie))
      {
        // client has no or unknown cookie

        cxxtools::Md5stream c;
        c << request.getSerial() << '-' << ::pthread_self() << '-' << rand();
        cookie = c.getHexDigest();
        log_debug("set Cookie " << cookie);
        reply.setCookie(currentSessionCookieName, cookie);
        putSessionScope(cookie, &request.getSessionScope());
      }
    }
    else
    {
      std::string cookie = request.getCookie(currentSessionCookieName);
      if (!cookie.empty())
      {
        // client has cookie
        log_debug("clear Cookie " << currentSessionCookieName);
        reply.clearCookie(currentSessionCookieName);
        removeSessionScope(cookie);
      }
    }
  }

  void ScopeManager::checkSessionTimeout()
  {
    time_t currentTime;
    time(&currentTime);
    cxxtools::MutexLock lock(sessionScopesMutex);
    sessionscopes_type::iterator it = sessionScopes.begin();
    unsigned count = 0;
    while (it != sessionScopes.end())
    {
      Sessionscope* s = it->second;
      if (currentTime - s->getAtime() > s->getTimeout())
      {
        log_info("sessiontimeout for session " << it->first << " reached");
        sessionscopes_type::iterator it2 = it;
        ++it;
        s->release();
        sessionScopes.erase(it2);
        ++count;
      }
      else
        ++it;
    }
    log_debug(count << " sessions timed out " << sessionScopes.size() << " sessions left");

  }
}
