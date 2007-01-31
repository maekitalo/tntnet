/* static.h
 * Copyright (C) 2003 Tommi Maekitalo
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

#ifndef TNT_STATIC_H
#define TNT_STATIC_H

#include <tnt/component.h>
#include <tnt/tntconfig.h>
#include <mime.h>

namespace tnt
{
  class Urlmapper;
  class Comploader;

  //////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Static : public tnt::Component, public MimeBase
  {
      static std::string documentRoot;

    public:
      virtual unsigned operator() (tnt::HttpRequest& request,
        tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
      virtual void drop();

      static void setDocumentRoot(const std::string& s)
        { documentRoot = s; }
      static const std::string& getDocumentRoot()
        { return documentRoot; }
  };
}

#endif // TNT_STATIC_H

