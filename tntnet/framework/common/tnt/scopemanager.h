/* tnt/scopemanager.h
 * Copyright (C) 2003-2006 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
