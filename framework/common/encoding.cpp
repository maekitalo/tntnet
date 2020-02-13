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
#include <cxxtools/log.h>
#include <cctype>

log_define("tnt.encoding")

namespace tnt
{
  namespace
  {
    enum State {
      state_0,
      state_encoding,
      state_encoding_sp,
      state_quality,
      state_qualityq,
      state_qualityeq,
      state_qualitypoint,
      state_qualitytenths,
      state_qualityhundredths,
      state_qualitythousandths,
      state_quality_sp
    };

    void throwInvalidHeader(const char* header, const char* p, State state)
    {
      log_warn("invalid encoding string <" << header << "> at position " << (p - header) << " in state " << static_cast<int>(state) << " ok <" << std::string(header, p - header) << '>');
      throwRuntimeError(std::string("invalid accept-encoding string \"") + header + '"');
    }
  }

  void Encoding::parse(const char* header)
  {
    _encodingMap.clear();

    if (header == 0)
      return;

    log_debug("encoding header <" << header << '>');
    State state = state_0;

    std::string encoding;
    unsigned quality = 0;
    const char* p;
    for (p = header; *p; ++p)
    {
      char ch = *p;
      switch (state)
      {
        case state_0:
          if (!std::isspace(ch))
          {
            encoding.clear();
            encoding.reserve(8);
            encoding += ch;
            state = state_encoding;
          }
          break;

        case state_encoding:
          if (ch == ';')
            state = state_qualityq;
          else if (ch == ',')
          {
            log_debug("encoding <" << encoding << "> quality 1000");
            _encodingMap.insert(encodingMapType::value_type(encoding, 1000));
            state = state_0;
          }
          else if (std::isspace(ch))
              state = state_encoding_sp;
          else
            encoding += ch;
          break;

        case state_encoding_sp:
          if (ch == ';')
            state = state_qualityq;
          else if (ch == ',')
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, 1000));
            state = state_0;
          }
          else if (std::isspace(ch))
            ;
          else
            throwInvalidHeader(header, p, state);
          break;

        case state_qualityq:
          if (ch == 'q')
            state = state_qualityeq;
          else if (!std::isspace(ch))
            throwInvalidHeader(header, p, state);
          break;

        case state_qualityeq:
          if (ch == '=')
            state = state_quality;
          else if (!std::isspace(ch))
            throwInvalidHeader(header, p, state);
          break;

        case state_quality:
          if (ch == '0')
          {
            quality = 0;
            state = state_qualitypoint;
          }
          else if (ch == '1')
          {
            quality = 1000;
            state = state_qualitypoint;
          }
          else if (!std::isspace(ch))
            throwInvalidHeader(header, p, state);
          break;

        case state_qualitypoint:
          if (ch == '.')
            state = state_qualitytenths;
          else if (ch == ',')
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_0;
          }
          else if (std::isspace(ch))
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_quality_sp;
          }
          else
            throwInvalidHeader(header, p, state);
          break;

        case state_quality_sp:
          if (ch == ',')
            state = state_0;
          else if (!std::isspace(ch))
            throwInvalidHeader(header, p, state);
          break;

        case state_qualitytenths:
          if (std::isdigit(ch))
          {
            quality += (ch - '0') * 100;
            state = state_qualityhundredths;
          }
          else if (std::isspace(ch))
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_quality_sp;
          }
          break;

        case state_qualityhundredths:
          if (std::isdigit(ch))
          {
            quality += (ch - '0') * 10;
            state = state_qualitythousandths;
          }
          else if (ch == ',')
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_0;
          }
          else if (std::isspace(ch))
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_quality_sp;
          }
          break;

        case state_qualitythousandths:
          if (std::isdigit(ch))
          {
            quality += (ch - '0');
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_quality_sp;
          }
          else if (ch == ',')
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_0;
          }
          else if (std::isspace(ch))
          {
            log_debug("encoding <" << encoding << "> quality " << quality);
            _encodingMap.insert(encodingMapType::value_type(encoding, quality));
            state = state_quality_sp;
          }
          else
            throwInvalidHeader(header, p, state);
          break;
      }
    }

    switch (state)
    {
      case state_encoding:
      case state_encoding_sp:
        log_debug("encoding <" << encoding << "> quality 1000");
        _encodingMap.insert(encodingMapType::value_type(encoding, 1000));
        break;

      case state_quality:
      case state_qualitypoint:
      case state_qualitytenths:
      case state_qualityhundredths:
      case state_qualitythousandths:
        log_debug("encoding <" << encoding << "> quality " << quality);
        _encodingMap.insert(encodingMapType::value_type(encoding, quality));
        break;

      case state_qualityq:
      case state_qualityeq:
        throwInvalidHeader(header, p, state);

      case state_0:
      case state_quality_sp:
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

    // return 1000 (accept), if encoding is identity, 0 otherwise
    return encoding == "identity" ? 1001 : 0;
  }
}

