/* langlib.cpp
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
