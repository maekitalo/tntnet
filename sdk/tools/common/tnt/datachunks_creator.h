/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#ifndef TNT_DATACHUNKS_CREATOR_H
#define TNT_DATACHUNKS_CREATOR_H

#include <list>
#include <string>

namespace tnt
{
  class DatachunksCreator
  {
      typedef std::list<std::string> chunks_type;

      chunks_type _chunks;
      mutable std::string _chunkCache;

      void createChunks() const;

    public:
      typedef chunks_type::const_reference const_reference;
      typedef chunks_type::reference reference;
      typedef chunks_type::value_type value_type;

      void push_back(const std::string& data)
      {
        _chunks.push_back(data);
        _chunkCache.clear();
      }
      void push_back(const char* data, unsigned len)
        { push_back(std::string(data, len)); }

      const char* ptr() const
      {
        if (_chunkCache.empty())
          createChunks();
        return _chunkCache.data();
      }

      unsigned count() const { return _chunks.size(); }

      unsigned size() const
      {
        if (_chunkCache.empty())
          createChunks();
        return _chunkCache.size();
      }

      bool empty() const           { return _chunks.empty(); }
      operator const char*() const { return ptr(); }
  };
}

#endif // TNT_DATACHUNKS_CREATOR_H

