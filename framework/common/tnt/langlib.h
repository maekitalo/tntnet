/*
 * Copyright (C) 2006 Tommi Maekitalo
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


#ifndef TNT_LANGLIB_H
#define TNT_LANGLIB_H

#include <string>
#include <map>
#include <set>
#include <tnt/unzipfile.h>
#include <cxxtools/mutex.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>

namespace tnt
{
  class LangLib : public cxxtools::RefCounted
  {
    private:
      unzipFile _file;
      std::string _lang;
      typedef std::map<std::string, std::string> dataMapType;
      typedef std::set<std::string> notFoundType;
      dataMapType _dataMap;
      notFoundType _notFound;
      cxxtools::ReadWriteMutex _mutex;

    public:
      LangLib() { }

      LangLib(const std::string& lib, const std::string& lang);

      const char* getData(const std::string& compname);

      typedef cxxtools::SmartPtr<LangLib> PtrType;
  };
}

#endif // TNT_LANGLIB_H

