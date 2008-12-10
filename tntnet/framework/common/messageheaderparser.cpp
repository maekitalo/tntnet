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


#include <tnt/messageheaderparser.h>
#include <cctype>
#include <cxxtools/log.h>

namespace tnt
{
  namespace
  {
    std::string chartoprint(char ch)
    {
      const static char hex[] = "0123456789abcdef";
      if (std::isprint(ch))
        return std::string(1, '\'') + ch + '\'';
      else
        return std::string("'\\x") + hex[ch >> 4] + hex[ch & 0xf] + '\'';
    }
  }

  log_define("tntnet.messageheader.parser")

  #define SET_STATE(new_state)  state = &Parser::new_state

  bool Messageheader::Parser::state_0(char ch)
  {
    if (ch >= 33 && ch <= 126 && ch != ':')
    {
      fieldname.clear();
      fieldname.reserve(16);
      fieldname += ch;
      SET_STATE(state_fieldname);
    }
    else if (ch == '\n')
      return true;
    else if (ch == '\r')
      SET_STATE(state_cr);
    else if (!std::isspace(ch))
    {
      log_warn("invalid character " << chartoprint(ch));
      failedFlag = true;
      return true;
    }

    return false;
  }

  bool Messageheader::Parser::state_cr(char ch)
  {
    if (ch != '\n')
    {
      log_warn("invalid character " << chartoprint(ch) << " in state-cr");
      failedFlag = true;
    }
    return true;
  }

  bool Messageheader::Parser::state_fieldname(char ch)
  {
    if (ch == ':')            // Field-name:
    {
      fieldname += ch;
      SET_STATE(state_fieldbody0);
    }
    else if (ch >= 33 && ch <= 126)
      fieldname += ch;
    else if (std::isspace(ch))
      SET_STATE(state_fieldnamespace);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in fieldname");
      failedFlag = true;
      return true;
    }
    return false;
  }

  bool Messageheader::Parser::state_fieldnamespace(char ch)
  {
    if (ch == ':')                   // "Field-name :"
      SET_STATE(state_fieldbody0);
    else if (ch >= 33 && ch <= 126)  // "Field-name blah..."
    {
      fieldbody.reserve(32);
      fieldbody += ch;
      SET_STATE(state_fieldbody);
    }
    else if (!std::isspace(ch))
    {
      log_warn("invalid character " << chartoprint(ch) << " in fieldname-space");
      failedFlag = true;
      return true;
    }
    return false;
  }

  bool Messageheader::Parser::state_fieldbody0(char ch)
  {
    if (ch == '\r')
      SET_STATE(state_fieldbody_cr);
    else if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else if (!std::isspace(ch))
    {
      fieldbody.clear();
      fieldbody.reserve(32);
      fieldbody += ch;
      SET_STATE(state_fieldbody);
    }
    return false;
  }

  bool Messageheader::Parser::state_fieldbody(char ch)
  {
    if (ch == '\r')
      SET_STATE(state_fieldbody_cr);
    else if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else
      fieldbody += ch;
    return false;
  }

  bool Messageheader::Parser::state_fieldbody_cr(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in fieldbody-cr");
      failedFlag = true;
      return true;
    }
    return false;
  }

  bool Messageheader::Parser::state_fieldbody_crlf(char ch)
  {
    if (ch == '\r')
      SET_STATE(state_end_cr);
    else if (ch == '\n')
    {
      log_debug("header " << fieldname << ": " << fieldbody);
      switch (header.onField(fieldname, fieldbody))
      {
        case OK:
        case END:  return true;
                   break;
        case FAIL: failedFlag = true;
                   log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
                   break;
      }
      fieldname.clear();
      fieldbody.clear();
      return true;
    }
    else if (std::isspace(ch))
    {
      fieldbody += ch;
      SET_STATE(state_fieldbody);
    }
    else if (ch >= 33 && ch <= 126)
    {
      switch (header.onField(fieldname, fieldbody))
      {
        case OK:   SET_STATE(state_fieldname);
                   break;
        case FAIL: failedFlag = true;
                   log_warn("invalid character " << chartoprint(ch) << " in fieldbody");
                   break;
        case END:  return true;
                   break;
      }
      fieldbody.clear();
      fieldname.clear();
      fieldname.reserve(16);
      fieldname += ch;
    }
    return false;
  }

  bool Messageheader::Parser::state_end_cr(char ch)
  {
    if (ch == '\n')
    {
      if (header.onField(fieldname, fieldbody) == FAIL)
      {
        log_warn("invalid header " << fieldname << ' ' << fieldbody);
        failedFlag = true;
      }

      fieldname.clear();
      fieldbody.clear();
      return true;
    }
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in end-cr");
      failedFlag = true;
      return true;
    }
    return false;
  }

  void Messageheader::Parser::reset()
  {
    failedFlag = false;
    SET_STATE(state_0);
  }
}
