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


#include "tnt/encoding.h"
#include "tnt/util.h"
#include <cctype>

namespace tnt
{
  namespace
  {
    void throwInvalidHeader(const char* header)
    {
      throwRuntimeError(std::string("invalid encoding-string \"") + header + '"');
    }
  }

  void Encoding::parse(const char* header)
  {
    _encodingMap.clear();

    if (header == 0)
      return;

    enum {
      state_0,
      state_encoding,
      state_quality,
      state_qualityq,
      state_qualityeq,
      state_qualitypoint,
      state_qualitytenth,
      state_qualityign
    } state = state_0;

    std::string encoding;
    unsigned quality = 0;
    for (const char* p = header; *p; ++p)
    {
      char ch = *p;
      switch (state)
      {
        case state_0:
          if (!std::isspace(ch))
          {
            encoding.clear();
            encoding.reserve(16);
            encoding += ch;
            state = state_encoding;
          }
          break;

        case state_encoding:
          if (ch == ';')
            state = state_qualityq;
          else if (ch == ',')
          {
            _encodingMap.insert(encodingMapType::value_type(encoding, 1));
            state = state_0;
          }
          else
            encoding += ch;
          break;

        case state_qualityq:
          if (ch == 'q')
            state = state_qualityeq;
          else if (!std::isspace(ch))
            throwInvalidHeader(header);
          break;

        case state_qualityeq:
          if (ch == '=')
            state = state_quality;
          else if (!std::isspace(ch))
            throwInvalidHeader(header);
          break;

        case state_quality:
          if (std::isdigit(ch))
          {
            quality = (ch - '0') * 10;
            state = state_qualitypoint;
          }
          else
            throwInvalidHeader(header);
          break;

        case state_qualitypoint:
          if (ch == '.')
            state = state_qualitytenth;
          else if (ch == ';')
          {
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_0;
          }
          else
            throwInvalidHeader(header);
          break;

        case state_qualitytenth:
          if (std::isdigit(ch))
          {
            quality += ch - '0';
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_qualityign;
          }
          else if (ch == ';')
            state = state_0;
          break;

        case state_qualityign:
          if (ch == ';')
            state = state_0;
          break;
      }
    }

    switch (state)
    {
      case state_encoding:
        _encodingMap.insert(encodingMapType::value_type(encoding, 1));
        break;

      case state_quality:
      case state_qualitypoint:
      case state_qualitytenth:
        _encodingMap.insert(encodingMapType::value_type(encoding, quality));
        break;

      default:
        break;
    }
  }

  unsigned Encoding::accept(const std::string& encoding) const
  {
    // check, if encoding is specified

    encodingMapType::const_iterator it = _encodingMap.find(encoding);
    if (it != _encodingMap.end())
      return it->second;

    // check, if a wildcard-rule is specified
    it = _encodingMap.find("*");
    if (it != _encodingMap.end())
      return it->second;

    // return 10 (accept), if encoding is identity, 0 otherwise
    return encoding == "identity" ? 10 : 0;
  }
}

