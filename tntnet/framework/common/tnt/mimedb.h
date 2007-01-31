/* tnt/mimedb.h
 * Copyright (C) 2005 Tommi Maekitalo
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
 */

#ifndef TNT_MIMEDB_H
#define TNT_MIMEDB_H

#include <map>
#include <string>
#include <iosfwd>

namespace tnt
{
  class MimeDb
  {
      typedef std::map<std::string, std::string> mimedb_type;
      mimedb_type mimeDb;

    public:
      typedef mimedb_type::key_type key_type;
      typedef mimedb_type::value_type value_type;
      typedef mimedb_type::size_type size_type;
      typedef mimedb_type::const_iterator const_iterator;
      typedef mimedb_type::iterator iterator;

      MimeDb()  {}
      explicit MimeDb(const std::string& mimefile)
        { read(mimefile); }
      explicit MimeDb(const char* mimefile)
        { read(mimefile); }

      void read(const std::string& mimefile);
      void read(const char* mimefile);
      void read(std::istream& in);

      size_type erase(const key_type& k)       { return mimeDb.erase(k); }
      void erase(iterator p)                   { mimeDb.erase(p); }
      void erase(iterator p, iterator q)       { mimeDb.erase(p, q); }
      void clear()                             { mimeDb.clear(); }
      iterator find(const key_type& k)         { return mimeDb.find(k); }
      const_iterator find(const key_type& k) const
                                               { return mimeDb.find(k); }
      const_iterator begin() const             { return mimeDb.begin(); }
      const_iterator end() const               { return mimeDb.end(); }
      iterator begin()                         { return mimeDb.begin(); }
      iterator end()                           { return mimeDb.end(); }
      size_type size() const                   { return mimeDb.size(); }
      bool empty() const                       { return mimeDb.empty(); }
      std::pair<iterator, bool> insert(const value_type& x)
                                               { return mimeDb.insert(x); }

      std::string getMimetype(const std::string& fname) const;
  };
}

#endif // TNT_MIMEDB_H

