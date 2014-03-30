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


#ifndef TNT_MESSAGEHEADER_H
#define TNT_MESSAGEHEADER_H

#include <istream>
#include <string>
#include <cstring>
#include <utility>

namespace tnt
{
  /// Standard-message-header like rfc822
  class Messageheader
  {
    public:
      static const unsigned MAXHEADERSIZE = 4096;

    private:
      char _rawdata[MAXHEADERSIZE];  // key_1\0value_1\0key_2\0value_2\0...key_n\0value_n\0\0
      unsigned _endOffset;

      char* getEnd() { return _rawdata + _endOffset; }

    public:
      class Parser;

      typedef std::pair<const char*, const char*> value_type;

      class const_iterator : public std::iterator<std::forward_iterator_tag, value_type>
      {
        friend class Messageheader;

        private:
          value_type _current_value;

          void fixup()
          {
            if (*_current_value.first)
              _current_value.second = _current_value.first + std::strlen(_current_value.first) + 1;
            else
              _current_value.first = _current_value.second = 0;
          }

          void moveForward()
          {
            _current_value.first = _current_value.second + std::strlen(_current_value.second) + 1;
            fixup();
          }

        public:
          const_iterator()
            : _current_value(static_cast<const char*>(0), static_cast<const char*>(0))
            { }

          explicit const_iterator(const char* p)
            : _current_value(p, p)
            { fixup(); }

          bool operator== (const const_iterator& it) const
            { return _current_value.first == it._current_value.first; }

          bool operator!= (const const_iterator& it) const
            { return _current_value.first != it._current_value.first; }

          const_iterator& operator++()
          {
            moveForward();
            return *this;
          }

          const_iterator operator++(int)
          {
            const_iterator ret = *this;
            moveForward();
            return ret;
          }

          const value_type& operator* () const  { return _current_value; }
          const value_type* operator-> () const { return &_current_value; }
      };

    protected:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

      virtual return_type onField(const char* name, const char* value);

    public:
      Messageheader()
        { clear(); }

      virtual ~Messageheader() { }

      const_iterator begin() const
        { return const_iterator(_rawdata); }
      const_iterator end() const
        { return const_iterator(); }

      const_iterator find(const char* key) const;
      const_iterator find(const std::string& key) const
        { return find(key.c_str()); }

      bool hasHeader(const char* key) const
        { return find(key) != end(); }
      bool hasHeader(const std::string& key) const
        { return hasHeader(key.c_str()); }

      bool compareHeader(const char* key, const char* value) const;
      bool compareHeader(const std::string& key, const std::string& value) const
        { return compareHeader(key.c_str(), value.c_str()); }

      void removeHeader(const char* key);
      void removeHeader(const std::string& key)
        { removeHeader(key.c_str()); }

      void clear();

      void setHeader(const char* key, const char* value, bool replace);

      void setHeader(const std::string& key, const std::string& value, bool replace)
        { setHeader(key.c_str(), value.c_str(), replace); }
  };

  std::istream& operator>> (std::istream& in, Messageheader& data);
}

#endif // TNT_MESSAGEHEADER_H

