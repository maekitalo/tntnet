/* langlib.cpp
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

#include "tnt/langlib.h"
#include <cxxtools/log.h>
#include <sstream>

log_define("tntnet.langlib")

namespace tnt
{
  LangLib::LangLib(const std::string lib, const std::string& lang_)
    : file(lib + '.' + lang_),
      lang(lang_)
  { }

  const char* LangLib::getData(const std::string& compname)
  {
    cxxtools::RdLock lock(monitor);
    dataMapType::const_iterator it = dataMap.find(compname);
    if (it == dataMap.end())
    {
      if (notFound.find(compname) != notFound.end())
      {
        log_debug("component \"" << compname << "\" not found in languagelibrary for lang=\"" << lang << '"');
        return 0;
      }

      lock.unlock();
      cxxtools::WrLock wrlock(monitor);

      try
      {
        unzipFileStream fileStream(file, compname + ".tntdata", true);
        std::ostringstream data;
        data << fileStream.rdbuf();
        it = dataMap.insert(dataMapType::value_type(compname, data.str())).first;
      }
      catch (const unzipEndOfListOfFile& e)
      {
        log_warn("component \"" << compname << "\" not found in languagelibrary for lang=\"" << lang << '"');
        notFound.insert(compname);
        return 0;
      }
    }

    return it->second.data();
  }
}
