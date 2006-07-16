/* encoding.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#include "tnt/encoding.h"
#include <cctype>
#include <cxxtools/log.h>
#include <stdexcept>

log_define("tntnet.encoding")

namespace tnt
{
  void Encoding::parse(const std::string& header)
  {
    log_debug("encode header \"" << header << '"');

    encodingMap.clear();

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
    unsigned quality;
    for (std::string::const_iterator it = header.begin(); it != header.end(); ++it)
    {
      char ch = *it;
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
            log_debug("encoding=" << encoding);
            encodingMap.insert(encodingMapType::value_type(encoding, 1));
            state = state_0;
          }
          else
            encoding += ch;
          break;

        case state_qualityq:
          if (ch == 'q')
            state = state_qualityeq;
          else if (!std::isspace(ch))
            throw std::runtime_error("invalid encoding-string \"" + header + '"');
          break;

        case state_qualityeq:
          if (ch == '=')
            state = state_quality;
          else if (!std::isspace(ch))
            throw std::runtime_error("invalid encoding-string \"" + header + '"');
          break;

        case state_quality:
          if (std::isdigit(ch))
          {
            quality = (ch - '0') * 10;
            state = state_qualitypoint;
          }
          else
            throw std::runtime_error("invalid encoding-string \"" + header + '"');
          break;

        case state_qualitypoint:
          if (ch == '.')
            state = state_qualitytenth;
          else if (ch == ';')
          {
            log_debug("encoding=" << encoding << " quality " << quality);
            encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_0;
          }
          else
            throw std::runtime_error("invalid encoding-string \"" + header + '"');
          break;

        case state_qualitytenth:
          if (std::isdigit(ch))
          {
            quality += ch - '0';
            log_debug("encoding=" << encoding << " quality " << quality);
            encodingMap.insert(encodingMapType::value_type(encoding, quality));
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
        log_debug("encoding=" << encoding);
        encodingMap.insert(encodingMapType::value_type(encoding, 1));
        break;

      case state_quality:
      case state_qualitypoint:
      case state_qualitytenth:
        log_debug("encoding=" << encoding << " quality " << quality);
        encodingMap.insert(encodingMapType::value_type(encoding, quality));
        break;

      default:
        break;
    }
  }

  unsigned Encoding::accept(const std::string& encoding) const
  {
    // check, if encoding is specified

    log_debug("accept(\"" << encoding << "\")");

    encodingMapType::const_iterator it =  encodingMap.find(encoding);
    if (it != encodingMap.end())
    {
      log_debug("accept(\"" << encoding << "\") => " << it->second);
      return it->second;
    }

    // check, if a wildcard-rule is specified
    it = encodingMap.find("*");
    if (it != encodingMap.end())
    {
      log_debug("accept(\"" << encoding << "\") => " << it->second);
      return it->second;
    }

    // return 10 (accept), if encoding is identity, 0 otherwise
    log_debug("accept(\"" << encoding << "\") => " << (encoding == "identity" ? 10 : 0));
    return encoding == "identity" ? 10 : 0;
  }

}
