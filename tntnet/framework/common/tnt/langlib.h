/* tnt/langlib.h
   Copyright (C) 2006 Tommi Maekitalo

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

#ifndef TNT_LANGLIB_H
#define TNT_LANGLIB_H

#include <string>
#include <map>
#include <set>
#include <tnt/unzipfile.h>
#include <cxxtools/thread.h>

namespace tnt
{
  class LangLib
  {
      unzipFile file;
      std::string lang;
      typedef std::map<std::string, std::string> dataMapType;
      typedef std::set<std::string> notFoundType;
      dataMapType dataMap;
      notFoundType notFound;
      cxxtools::RWLock monitor;

    public:
      LangLib()
        { }

      LangLib(const std::string lib, const std::string& lang_);

      const char* getData(const std::string& compname);
  };
}

#endif // TNT_LANGLIB_H

