/* tnt/datachunks_creator.h
 * Copyright (C) 2003 Tommi Maekitalo
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


#ifndef TNT_DATACHUNKS_CREATOR_H
#define TNT_DATACHUNKS_CREATOR_H

#include <list>
#include <string>

namespace tnt
{
  class DatachunksCreator
  {
      typedef std::list<std::string> chunks_type;
      chunks_type chunks;
      mutable std::string chunks_cache;

      void createChunks() const;

    public:
      typedef chunks_type::const_reference const_reference;
      typedef chunks_type::reference reference;
      typedef chunks_type::value_type value_type;

      void push_back(const std::string& data)
        { chunks.push_back(data); chunks_cache.clear(); }
      void push_back(const char* data, unsigned len)
        { push_back(std::string(data, len)); }

      const char* ptr() const
      {
        if (chunks_cache.empty())
          createChunks();
        return chunks_cache.data();
      }

      unsigned count() const
      { return chunks.size(); }

      unsigned size() const
      {
        if (chunks_cache.empty())
          createChunks();
        return chunks_cache.size();
      }

      bool empty() const
      { return chunks.empty(); }

      operator const char*() const
      { return ptr(); }
  };
}

#endif // TNT_DATACHUNKS_CREATOR_H

