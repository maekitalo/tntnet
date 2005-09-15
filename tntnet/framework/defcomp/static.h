/* static.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef STATIC_H
#define STATIC_H

#include <tnt/component.h>
#include <tnt/tntconfig.h>

namespace tnt
{
  class Urlmapper;
  class Comploader;

  //////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Static : public tnt::Component
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

#endif // STATIC_H

