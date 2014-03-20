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


#include "tnt/langlib.h"
#include <cxxtools/log.h>
#include <sstream>

log_define("tntnet.langlib")

namespace tnt
{
  LangLib::LangLib(const std::string& lib, const std::string& lang)
    : _file(lib + '.' + lang),
      _lang(lang)
  { }

  const char* LangLib::getData(const std::string& compname)
  {
    cxxtools::ReadLock lock(_mutex);
    dataMapType::const_iterator it = _dataMap.find(compname);
    if (it == _dataMap.end())
    {
      if (_notFound.find(compname) != _notFound.end())
      {
        log_debug("component \"" << compname << "\" not found in languagelibrary for lang=\"" << _lang << '"');
        return 0;
      }

      lock.unlock();
      cxxtools::WriteLock wrlock(_mutex);

      try
      {
        unzipFileStream fileStream(_file, compname + ".tntdata", true);
        std::ostringstream data;
        data << fileStream.rdbuf();
        it = _dataMap.insert(dataMapType::value_type(compname, data.str())).first;
      }
      catch (const unzipEndOfListOfFile& e)
      {
        log_warn("component \"" << compname << "\" not found in languagelibrary for lang=\"" << _lang << '"');
        _notFound.insert(compname);
        return 0;
      }
    }

    return it->second.data();
  }
}

