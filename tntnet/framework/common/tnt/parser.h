/* tnt/parser.h
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

#ifndef TNT_PARSER_H
#define TNT_PARSER_H

#include <iostream>

namespace tnt
{
  template <typename this_type>
  class Parser
  {
    protected:

      typedef bool (this_type::*state_type)(char);
      state_type state;

      bool failedFlag;

      virtual bool post(bool ret)  { return ret; }

    public:
      explicit Parser(state_type initialState)
        : state(initialState),
          failedFlag(false)
        { }
      virtual ~Parser() {}

      bool parse(char ch)
      {
        return post( (static_cast<this_type*>(this)->*state)(ch) );
      }

      bool parse(const char* str, unsigned size)
      {
        for (unsigned s = 0; s < size; ++s)
          if (parse(str[s]))
            return true;
        return false;
      }

      bool parse(std::istream& in)
      {
        std::streambuf* buf = in.rdbuf();
        while (buf->sgetc() != std::ios::traits_type::eof())
          if ( parse(buf->sbumpc()) )
            return true;

        in.setstate(std::ios::eofbit);
        return false;
      }

      bool failed() const
      {
        return failedFlag;
      }
  };
}

#endif // TNT_PARSER_H
