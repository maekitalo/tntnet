/* messageattribute.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include <tnt/messageattribute.h>
#include <iostream>

namespace
{
  bool istokenchar(char ch)
  {
    return ch >= 33
        && ch <= 126
        && ch != '(' && ch != ')' && ch != '<' && ch != '>'  && ch != '@'
        && ch != ',' && ch != ';' && ch != ':' && ch != '\\' && ch != '"'
        && ch != '/' && ch != '[' && ch != ']' && ch != '?'  && ch != '=';
  }

  bool isblank(char ch)
  {
    return ch == ' ' || ch == '\t';
  }
}

namespace tnt
{
  void messageattribute_parser::parse(std::istream& in)
  {
    enum state_type
    {
      state_0,
      state_type0,
      state_type_sp,
      state_subtype0,
      state_subtype,
      state_subtype_sp,
      state_attribute0,
      state_attribute,
      state_attribute_sp,
      state_value0,
      state_value,
      state_qvalue,
      state_end
    };

    state_type state = state_0;
    std::streambuf* buf = in.rdbuf();

    std::string type, subtype, attribute, value;

    while (state != state_end && buf->sgetc() != std::ios::traits_type::eof())
    {
      char ch = buf->sgetc();
      switch (state)
      {
        case state_0:
          if (istokenchar(ch))
          {
            type = ch;
            state = state_type0;
          }
          else if (std::isspace(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_type0:
          if (isblank(ch))
            state = state_type_sp;
          else if (ch == '/')
            state = state_subtype0;
          else if (ch == ';')
          {
            switch (onType(type, std::string()))
            {
              case OK:   state = state_attribute0; break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else if (istokenchar(ch))
            type += ch;
          else
            in.setstate(std::ios::failbit);
          break;

        case state_type_sp:
          if (ch == '/')
            state = state_subtype0;
          else if (ch == ';')
          {
            switch (onType(type, std::string()))
            {
              case OK:   state = state_attribute0; break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else if (!isblank(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_subtype0:
          if (istokenchar(ch))
          {
            subtype = ch;
            state = state_subtype;
          }
          else if (!isblank(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_subtype:
          if (ch == ';')
          {
            switch (onType(type, subtype))
            {
              case OK:   state = state_attribute0; break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else if (isblank(ch))
          {
            switch (onType(type, subtype))
            {
              case OK:   state = state_subtype_sp; break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else if (istokenchar(ch))
            subtype += ch;
          else
            in.setstate(std::ios::failbit);
          break;

        case state_subtype_sp:
          if (ch == ';')
            state = state_attribute0;
          else if (!isblank(ch))
            state = state_end;
          break;

        case state_attribute0:
          if (istokenchar(ch))
          {
            attribute = ch;
            state = state_attribute;
          }
          break;

        case state_attribute:
          if (istokenchar(ch))
            attribute += ch;
          else if (isblank(ch))
            state = state_attribute_sp;
          else if (ch == '=')
            state = state_value0;
          else
            in.setstate(std::ios::failbit);
          break;

        case state_attribute_sp:
          if (ch == '=')
            state = state_value0;
          else if (!isblank(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_value0:
          if (ch == '"')
            state = state_qvalue;
          else if (istokenchar(ch))
          {
            value = ch;
            state = state_value;
          }
          else
            in.setstate(std::ios::failbit);
          break;

        case state_qvalue:
          if (ch == '"')
          {
            switch (onParameter(attribute, value))
            {
              case OK:   state = state_subtype_sp;
                         attribute.clear();
                         value.clear();
                         break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else
            value += ch;
          break;

        case state_value:
          if (istokenchar(ch))
            value += ch;
          else if (isblank(ch))
          {
            switch (onParameter(attribute, value))
            {
              case OK:   state = state_subtype_sp;
                         attribute.clear();
                         value.clear();
                         break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else if (ch == ';')
          {
            switch (onParameter(attribute, value))
            {
              case OK:   state = state_attribute0;
                         attribute.clear();
                         value.clear();
                         break;
              case FAIL: in.setstate(std::ios::failbit); break;
              case END:  state = state_end;  break;
            }
          }
          else
            in.setstate(std::ios::failbit);
          break;

        case state_end:
          break;
      }

      if (state != state_end)
        buf->sbumpc();
    }

    // process last parameter
    if (state == state_value
      && !in.fail()
      && onParameter(attribute, value) == FAIL)
        in.setstate(std::ios::failbit);

    if (buf->sgetc() == std::ios::traits_type::eof())
      in.setstate(std::ios::eofbit);
  }
}
