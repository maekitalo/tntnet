/* cache.cpp
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

#include "tnt/cache.h"

namespace tnt
{
  std::pair<bool, Cache::value_type> Cache::get(const key_type& key)
  {
    data_type::iterator it = data.find(key);
    if (it == data.end())
    {
      ++misses;
      return std::pair<bool, value_type>(false, value_type());
    }

    value_type ret = it->second.first;
    atime_type::iterator ait = it->second.second;

    if (ait != atime.begin())
    {
      atime_type::iterator prev = ait;
      --prev;
      atime.erase(ait);
      it->second.second = atime.insert(prev, key);
    }

    ++hits;
    return std::pair<bool, value_type>(true, ret);
  }

  void Cache::put(const key_type& key, const value_type& value)
  {
    size_type s = key.size() + value.size();

    data_type::iterator di = data.find(key);
    if (di != data.end())
      di->second.first = value;

    // does it fit?
    if (s > max_size)
      return;

    while (!atime.empty() && current_size + s > max_size)
    {
      // remove last element

      data_type::iterator di = data.find(atime.back());
      current_size -= di->first.size() + di->second.first.size();

      data.erase(atime.back());
      atime.pop_back();
    }

    // insert new element in the middle
    atime_type::iterator ipos = atime.begin();
    std::advance(ipos, atime.size() / 2);
    data[key] = data_type::mapped_type(value, atime.insert(ipos, key));
    current_size += s;
  }

  void Cache::setMaxSize(unsigned m)
  {
    max_size = m;
    while (!atime.empty() && current_size > max_size)
    {
      // remove last element

      data_type::iterator di = data.find(atime.back());
      current_size -= di->first.size() + di->second.first.size();

      data.erase(atime.back());
      atime.pop_back();
    }

  }
}
