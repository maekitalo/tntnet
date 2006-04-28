/* tnt/cache.h
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef TNT_CACHE_H
#define TNT_CACHE_H

#include <string>
#include <map>
#include <list>

namespace tnt
{
  class Cache
  {
    public:
      typedef std::string key_type;
      typedef std::string value_type;
      typedef std::string::size_type size_type;

    private:
      typedef std::list<key_type> atime_type;
      typedef std::map<key_type, std::pair<value_type, atime_type::iterator> > data_type;

      data_type data;
      atime_type atime;

      size_type max_size;
      size_type current_size;

      unsigned hits;
      unsigned misses;

    public:
      explicit Cache(size_type max_size_)
        : max_size(max_size_),
          current_size(0),
          hits(0),
          misses(0)
          { }

      std::pair<bool, value_type> get(const key_type& key);
      void put(const key_type& key, const value_type& value);

      size_type size() const        { return current_size; }
      size_type count() const       { return data.size(); }
      size_type getMaxSize() const  { return max_size; }
      void setMaxSize(unsigned m);
      unsigned getHits() const      { return hits; }
      unsigned getMisses() const    { return misses; }
  };
}

#endif // TNT_CACHE_H

