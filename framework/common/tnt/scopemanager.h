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


#ifndef TNT_SCOPEMANAGER_H
#define TNT_SCOPEMANAGER_H

#include <string>
#include <map>
#include <string>
#include <cxxtools/mutex.h>

namespace tnt
{
  class Scope;
  class Sessionscope;
  class HttpRequest;
  class HttpReply;

  /// @cond internal
  class ScopeManager
  {
    public:
      typedef std::map<std::string, Scope*> scopes_type;
      typedef std::map<std::string, Sessionscope*> sessionscopes_type;

    private:
      scopes_type _applicationScopes;
      sessionscopes_type _sessionScopes;
      cxxtools::Mutex _applicationScopesMutex;
      cxxtools::Mutex _sessionScopesMutex;

      Scope* getApplicationScope(const std::string& appname);
      Sessionscope* getSessionScope(const std::string& sessioncookie);
      bool hasSessionScope(const std::string& sessioncookie);
      void putSessionScope(const std::string& sessioncookie, Sessionscope* s);
      void removeApplicationScope(const std::string& appname);
      void removeSessionScope(const std::string& sessioncookie);

    public:
      ScopeManager();
      ~ScopeManager();

      void preCall(HttpRequest& request, const std::string& app);
      void setSessionId(HttpRequest& request, const std::string& sessionId);
      std::string postCall(HttpRequest& request, HttpReply& reply, const std::string& app);
      void checkSessionTimeout();
  };
  /// @endcond internal
}

#endif // TNT_SCOPEMANAGER_H

