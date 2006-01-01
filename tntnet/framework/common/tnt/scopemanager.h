/* tnt/scopemanager.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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
