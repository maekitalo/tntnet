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


#include <tnt/messageheader.h>
#include <tnt/messageheaderparser.h>
#include <cxxtools/log.h>
#include <stdexcept>
#include <tnt/stringlessignorecase.h>
#include <cstring>

namespace tnt
{
  log_define("tntnet.messageheader")

  const unsigned Messageheader::MAXHEADERSIZE;

  bool Messageheader::compareHeader(const char* key, const char* value) const
  {
    const_iterator it = find(key);
    return it == end() ? false
                       : tnt::StringCompareIgnoreCase<const char*>(
                           it->second, value) == 0;
  }

  void Messageheader::removeHeader(const char* key)
  {
    if (!*key)
      throw std::runtime_error("empty key not allowed in messageheader");

    char* p = getEnd();

    const_iterator it = begin();
    while (it != end())
    {
      if (StringCompareIgnoreCase<const char*>(key, it->first) == 0)
      {
        unsigned slen = it->second - it->first + std::strlen(it->second) + 1;

        std::memmove(
            const_cast<char*>(it->first),
            it->first + slen,
            p - it->first + slen);

        p -= slen;

        it.fixup();
      }
      else
        ++it;
    }

    _endOffset = p - _rawdata;
  }

  Messageheader::const_iterator Messageheader::find(const char* key) const
  {
    for (const_iterator it = begin(); it != end(); ++it)
    {
      if (StringCompareIgnoreCase<const char*>(key, it->first) == 0)
        return it;
    }

    return end();
  }

  void Messageheader::clear()
  {
#ifdef DEBUG
    std::memset(rawdata, '\xfe', sizeof(rawdata));
#endif
    _rawdata[0] = _rawdata[1] = '\0';
    _endOffset = 0;
  }

  void Messageheader::setHeader(const char* key, const char* value, bool replace)
  {
    if (!*key)
      throw std::runtime_error("empty key not allowed in messageheader");

    if (replace)
      removeHeader(key);

    char* p = getEnd();

    size_t lk = std::strlen(key);     // length of key
    size_t lk2 = key[lk-1] == ':' ? lk + 1 : lk + 2;  // length of key including trailing ':' and terminator
    size_t lv = std::strlen(value);   // length of value

    if (p - _rawdata + lk2 + lv + 3 > MAXHEADERSIZE)
      throw std::runtime_error("message header too big");

    std::strcpy(p, key);   // copy key
    p += lk2;
    *(p - 2) = ':';        // make sure, key is prepended by ':'
    *(p - 1) = '\0';
    std::strcpy(p, value); // copy value
    p[lv + 1] = '\0';      // put new message end marker in place

    _endOffset = (p + lv + 1) - _rawdata;
  }

  Messageheader::return_type Messageheader::onField(const char* name, const char* value)
  {
    log_debug(name << ' ' << value);
    return OK;
  }

  std::istream& operator>> (std::istream& in, Messageheader& data)
  {
    Messageheader::Parser p(data);
    p.parse(in);
    return in;
  }
}

