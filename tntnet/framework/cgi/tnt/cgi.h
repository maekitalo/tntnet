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

