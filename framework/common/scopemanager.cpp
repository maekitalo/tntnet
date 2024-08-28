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
namespace
{
    static std::string createSessionId(const tnt::HttpRequest& request)
    {
        cxxtools::Md5stream c;
        c << request.getSerial() << '-' << ::pthread_self() << '-' << rand();
        return c.getHexDigest();
    }

    static std::string sessionCookieName(const std::string& app, bool secure)
    {
        std::string name;
        if (secure)
            name = 's';

        if (app.empty())
            name += "tntnet";
        else
            name += app;

        return name;
    }
}

std::shared_ptr<Scope> ScopeManager::getApplicationScope(const std::string& appname)
{
    std::unique_lock<std::mutex> lock(_applicationScopesMutex);

    auto it = _applicationScopes.find(appname);
    if (it == _applicationScopes.end())
    {
        log_debug("applicationscope <" + appname + "> not found - create new");
        return _applicationScopes.emplace(appname, std::make_shared<Scope>()).first->second;
    }
    else
      log_debug("applicationscope <" + appname + "> found");

    return it->second;
}

std::shared_ptr<Sessionscope> ScopeManager::getSessionScope(const std::string& sessioncookie, bool create)
{
    log_debug("getSessionScope(\"" << sessioncookie << "\")");

    std::unique_lock<std::mutex> lock(_sessionScopesMutex);
    auto it = _sessionScopes.find(sessioncookie);
    if (it == _sessionScopes.end())
    {
        if (create)
        {
            log_debug("session " << sessioncookie << " not found - create new");
            return _sessionScopes.emplace(sessioncookie, std::make_shared<Sessionscope>()).first->second;
        }
        else
        {
            log_debug("session " << sessioncookie << " not found");
            return nullptr;
        }
    }
    else
    {
        log_debug("session " << sessioncookie << " found");
        it->second->touch();
        return it->second;
    }
}

void ScopeManager::preCall(HttpRequest& request, const std::string& app)
{
    // check session-cookie
    std::string currentSessionCookieName = sessionCookieName(app, false);
    std::string currentSecureSessionCookieName = sessionCookieName(app, true);

    Cookie c = request.getCookie(currentSessionCookieName);
    if (c.getValue().empty())
    {
        /*
        std::unique_lock<std::mutex> lock(sessionScopesMutex);
        log_debug(sessionScopes.size() << " sessions available");
        for (sessionscopes_type::iterator it = sessionScopes.begin(); it != sessionScopes.end(); ++it)
          log_debug("available session " << it->first << " value " << it->second);
          */

        log_debug("session cookie " << currentSessionCookieName << " not found - keep session");
    }
    else
    {
        log_debug("session cookie " << currentSessionCookieName << " found: " << c.getValue());
        request.setSessionScope(getSessionScope(c.getValue(), true));
    }

    if (request.isSsl())
    {
        c = request.getCookie(currentSecureSessionCookieName);
        if (c.getValue().empty())
        {
            log_debug("secure session cookie " << currentSecureSessionCookieName
                << " not found - keep session");
        }
        else if (request.isSsl())
        {
            log_debug("secure session cookie " << currentSecureSessionCookieName
                << " found: " << c.getValue());

            request.setSecureSessionScope(getSessionScope(c.getValue(), true));
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
        auto sessionScope = getSessionScope(sessionId, false);
        if (sessionScope)
        {
            log_debug("session found");
            request.setSessionScope(sessionScope);
        }
    }
}

std::string ScopeManager::postCall(HttpRequest& request, HttpReply& reply, const std::string& app)
{
    const auto& cfg = TntConfig::it();

    std::string currentSessionCookieName = sessionCookieName(app, false);
    std::string currentSecureSessionCookieName = sessionCookieName(app, true);

    std::string sessionId;

    if (reply.isClearSession())
    {
        std::unique_lock<std::mutex> lock(_sessionScopesMutex);

        sessionId = request.getCookie(currentSessionCookieName);
        if (!sessionId.empty())
            _sessionScopes.erase(sessionId);

        std::string secureSessionId = request.getCookie(currentSecureSessionCookieName);
        if (!secureSessionId.empty())
            _sessionScopes.erase(secureSessionId);
    }
    else
    {
        if (request.hasSessionScope())
        {
            // request has session scope
            sessionId = request.getCookie(currentSessionCookieName);
            if (sessionId.empty() || reply.isRenewSessionId())
            {
                // client has no sessionId or application requests new id

                std::string newSessionId = createSessionId(request);
                log_info("create new session " << newSessionId);
                tnt::Cookie cookie(newSessionId);
                cookie.setAttr(tnt::Cookie::sameSite, "strict")
                      .setAttr(tnt::Cookie::httpOnly);
                if (cfg.secureSession)
                    cookie.setSecure();
                reply.setCookie(currentSessionCookieName, cookie);

                std::unique_lock<std::mutex> lock(_sessionScopesMutex);
                _sessionScopes.emplace(newSessionId, request.getSessionScopePtr());

                if (reply.isRenewSessionId())
                    _sessionScopes.erase(sessionId);

                sessionId = newSessionId;
            }
            else
            {
                std::unique_lock<std::mutex> lock(_sessionScopesMutex);
                if (_sessionScopes.count(sessionId) == 0)
                {
                    // client has a sessionId but no associated session
                    // this may happen, when tntnet is restarted while the browser has a session

                    _sessionScopes.emplace(sessionId, request.getSessionScopePtr());
                }
            }
        }

        if (request.isSsl() && request.hasSecureSessionScope())
        {
            // request has secure session scope
            std::string sessionId = request.getCookie(currentSecureSessionCookieName);
            if (sessionId.empty() || reply.isRenewSessionId())
            {
                // client has no sessionId

                auto newSessionId = createSessionId(request);
                log_info("create new secure session " << newSessionId);
                tnt::Cookie cookie(newSessionId);
                cookie.setSecure()
                      .setAttr(tnt::Cookie::sameSite, "strict")
                      .setAttr(tnt::Cookie::httpOnly);
                reply.setCookie(currentSecureSessionCookieName, cookie);

                std::unique_lock<std::mutex> lock(_sessionScopesMutex);

                _sessionScopes.emplace(newSessionId, request.getSessionScopePtr());
                if (reply.isRenewSessionId())
                    _sessionScopes.erase(sessionId);
            }
            else
            {
                std::unique_lock<std::mutex> lock(_sessionScopesMutex);
                if (_sessionScopes.count(sessionId) == 0)
                {
                    // client has a sessionId but no associated session
                    // this may happen, when tntnet is restarted while the browser has a session

                    _sessionScopes.emplace(sessionId, request.getSecureSessionScopePtr());
                }
            }
        }
    }

    return sessionId;
}

void ScopeManager::checkSessionTimeout()
{
    time_t currentTime;
    time(&currentTime);
    std::unique_lock<std::mutex> lock(_sessionScopesMutex);
    auto it = _sessionScopes.begin();
    unsigned count = 0;
    while (it != _sessionScopes.end())
    {
        auto s = it->second;
        if (cxxtools::Seconds(currentTime - s->getAtime()) > s->getTimeout())
        {
            log_info("sessiontimeout for session " << it->first << " reached");
            auto it2 = it;
            ++it;
            _sessionScopes.erase(it2);
            ++count;
        }
        else
          ++it;
    }

}
}

