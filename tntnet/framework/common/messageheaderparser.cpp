/* messageheaderparser.cpp
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

#include <tnt/messageheaderparser.h>
#include <tnt/fastctype.h>
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
      fieldname = ch;
      SET_STATE(state_fieldname);
    }
    else if (ch == '\n')
      return true;
    else if (ch == '\r')
      SET_STATE(state_cr);
    else if (!myisspace(ch))
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
    else if (myisspace(ch))
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
      fieldbody = ch;
      SET_STATE(state_fieldbody);
    }
    else if (!myisspace(ch))
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
    else if (!myisspace(ch))
    {
      fieldbody = ch;
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
    else if (myisspace(ch))
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
      fieldname = ch;
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
