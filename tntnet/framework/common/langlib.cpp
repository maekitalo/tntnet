/* langlib.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
