/* tnt/scopemanager.h
 * Copyright (C) 2003-2006 Tommi Maekitalo
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


#ifndef TNT_SCOPEMANAGER_H
#define TNT_SCOPEMANAGER_H

#include <map>
#include <cxxtools/thread.h>

namespace tnt
{
  class Scope;
  class Sessionscope;
  class HttpRequest;
  class HttpReply;

  class ScopeManager
  {
    public:
      typedef std::map<std::string, Scope*> scopes_type;
      typedef std::map<std::string, Sessionscope*> sessionscopes_type;

    private:
      scopes_type applicationScopes;
      sessionscopes_type sessionScopes;
      cxxtools::Mutex applicationScopesMutex;
      cxxtools::Mutex sessionScopesMutex;

      Scope* getApplicationScope(const std::string& appname);
      Sessionscope* getSessionScope(const std::string& sessioncookie);
      bool hasSessionScope(const std::string& sessioncookie);
      void putSessionScope(const std::string& sessioncookie, Sessionscope* s);
      void removeApplicationScope(const std::string& appname);
      void removeSessionScope(const std::string& sessioncookie);

    public:
      void preCall(HttpRequest& request, const std::string& app);
      void postCall(HttpRequest& request, HttpReply& reply, const std::string& app);
      void checkSessionTimeout();
  };
}

#endif // TNT_SCOPEMANAGER_H
