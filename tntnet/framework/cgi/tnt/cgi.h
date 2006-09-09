/* tnt/cgi.h
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

#ifndef TNT_CGI_H
#define TNT_CGI_H

#include <tnt/httprequest.h>
#include <tnt/scopemanager.h>
#include <tnt/tntconfig.h>
#include <tnt/comploader.h>

namespace tnt
{
  /// enable cgi-interface for components
  class Cgi
  {
      std::string componentName;
      Tntconfig config;

      HttpRequest request;
      ScopeManager scopeManager;
      Comploader comploader;
      Scope threadScope;

      void getHeader(const char* env, const std::string& headername);
      void getMethod();
      void getQueryString();
      void getPathInfo();
      void getRemoteAddr();
      void readBody();
      void execute();

    public:
      Cgi(int argc, char* argv[]);
      bool isFastCgi() const;
      bool isCgi() const  { return !isFastCgi(); }
      int run();
      int runCgi();
      int runFCgi();
  };
}

#endif // TNT_CGI_H

