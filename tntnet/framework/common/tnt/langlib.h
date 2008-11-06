/* tnt/langlib.h
 * Copyright (C) 2006 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_LANGLIB_H
#define TNT_LANGLIB_H

#include <string>
#include <map>
#include <set>
#include <tnt/unzipfile.h>
#include <cxxtools/mutex.h>

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

