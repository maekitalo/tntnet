/* messageheaderparser.cpp
   Copyright (C) 2003-2005 Tommi Mäkitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <tnt/messageheaderparser.h>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.messageheader.parser");

  #define SET_STATE(new_state)  state = &parser::new_state

  bool messageheader::parser::state_0(char ch)
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
    else if (!std::isspace(ch))
    {
      failed_flag = true;
      return true;
    }

    return false;
  }

  bool messageheader::parser::state_cr(char ch)
  {
    if (ch != '\n')
      failed_flag = true;
    return failed();
  }

  bool messageheader::parser::state_fieldname(char ch)
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
      failed_flag = true;
      return true;
    }
    return false;
  }

  bool messageheader::parser::state_fieldnamespace(char ch)
  {
    if (ch == ':')                   // "Field-name :"
      SET_STATE(state_fieldbody0);
    else if (ch >= 33 && ch <= 126)  // "Field-name blah..."
    {
      fieldbody = ch;
      SET_STATE(state_fieldbody);
    }
    else if (!std::isspace(ch))
    {
      failed_flag = true;
      return true;
    }
    return false;
  }

  bool messageheader::parser::state_fieldbody0(char ch)
  {
    if (ch == '\r')
      SET_STATE(state_fieldbody_cr);
    else if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else if (!std::isspace(ch))
    {
      fieldbody = ch;
      SET_STATE(state_fieldbody);
    }
    return false;
  }

  bool messageheader::parser::state_fieldbody(char ch)
  {
    if (ch == '\r')
      SET_STATE(state_fieldbody_cr);
    else if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else
      fieldbody += ch;
    return false;
  }

  bool messageheader::parser::state_fieldbody_cr(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_fieldbody_crlf);
    else
    {
      failed_flag = true;
      return true;
    }
    return false;
  }

  bool messageheader::parser::state_fieldbody_crlf(char ch)
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
        case FAIL: failed_flag = true;
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
        case FAIL: failed_flag = true;
                   break;
        case END:  return true;
                   break;
      }
      fieldbody.clear();
      fieldname = ch;
    }
    return false;
  }

  bool messageheader::parser::state_end_cr(char ch)
  {
    if (ch == '\n')
    {
      if (header.onField(fieldname, fieldbody) == FAIL)
        failed_flag = true;

      fieldname.clear();
      fieldbody.clear();
      return true;
    }
    else
    {
      failed_flag = true;
      return true;
    }
    return false;
  }
}
