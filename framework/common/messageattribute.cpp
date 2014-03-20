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


#include <tnt/messageattribute.h>
#include <cctype>
#include <iostream>

namespace tnt
{
  namespace
  {
    static inline bool istokenchar(char ch)
    {
      return ch >= 33
          && ch <= 126
          && ch != '(' && ch != ')' && ch != '<' && ch != '>'  && ch != '@'
          && ch != ',' && ch != ';' && ch != ':' && ch != '\\' && ch != '"'
          && ch != '/' && ch != '[' && ch != ']' && ch != '?'  && ch != '=';
    }

    static inline bool tnt_isblank(char ch)
      { return ch == ' ' || ch == '\t'; }
  }

  void MessageattributeParser::parse(std::istream& in)
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
            type.clear();
            type.reserve(16);
            type += ch;
            state = state_type0;
          }
          else if (std::isspace(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_type0:
          if (tnt_isblank(ch))
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
          else if (!tnt_isblank(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_subtype0:
          if (istokenchar(ch))
          {
            subtype = ch;
            state = state_subtype;
          }
          else if (!tnt_isblank(ch))
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
          else if (tnt_isblank(ch))
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
          else if (!tnt_isblank(ch))
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
          else if (tnt_isblank(ch))
            state = state_attribute_sp;
          else if (ch == '=')
            state = state_value0;
          else
            in.setstate(std::ios::failbit);
          break;

        case state_attribute_sp:
          if (ch == '=')
            state = state_value0;
          else if (!tnt_isblank(ch))
            in.setstate(std::ios::failbit);
          break;

        case state_value0:
          if (ch == '"')
            state = state_qvalue;
          else if (istokenchar(ch))
          {
            value.clear();
            value.reserve(16);
            value += ch;
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
          else if (tnt_isblank(ch))
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

        case state_end: // not reachable but to satisfy the compiler we put it here
          break;
      }

      if (state != state_end)
        buf->sbumpc();
    }

    // process last parameter
    if (in.fail())
      return;

    switch (state)
    {
      case state_0:
      case state_type0:
        break;

      case state_type_sp:
      case state_subtype0:
      case state_subtype:
        if (onType(type, subtype) == FAIL)
          in.setstate(std::ios::failbit);
        break;

      case state_subtype_sp:
      case state_attribute0:
        break;

      case state_attribute:
      case state_attribute_sp:
      case state_value0:
      case state_qvalue:
        in.setstate(std::ios::failbit);
        break;

      case state_value:
        if(onParameter(attribute, value) == FAIL)
          in.setstate(std::ios::failbit);
        break;

      case state_end:
        break;
    };

    if (buf->sgetc() == std::ios::traits_type::eof())
      in.setstate(std::ios::eofbit);
  }
}

