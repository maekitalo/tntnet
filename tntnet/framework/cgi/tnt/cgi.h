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

#ifndef TNT_CGI_H
#define TNT_CGI_H

#include <tnt/httprequest.h>
#include <tnt/scopemanager.h>
#include <tnt/tntconfig.h>
#include <tnt/comploader.h>
#include <tnt/tntnet.h>

namespace tnt
{
  /// enable cgi-interface for components
  class Cgi
  {
      std::string componentName;
      Tntconfig config;

      Tntnet application;
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

