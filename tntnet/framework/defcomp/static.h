/* static.h
   Copyright (C) 2003 Tommi Mäkitalo

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
  class urlmapper;
  class comploader;
}

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_static(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
  bool config_static(const tnt::tntconfig::name_type& key,
    const tnt::tntconfig::params_type& values);
}

namespace tntcomp
{
  //////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class staticcomp : public tnt::component
  {
      static std::string document_root;

    protected:
      virtual ~staticcomp() { };

    public:
      virtual unsigned operator() (tnt::httpRequest& request,
        tnt::httpReply& reply, cxxtools::query_params& qparam);
      virtual bool drop();

      static void setDocumentRoot(const std::string& s)
      { document_root = s; }
      static const std::string& getDocumentRoot()
      { return document_root; }
  };
}

#endif // STATIC_H

