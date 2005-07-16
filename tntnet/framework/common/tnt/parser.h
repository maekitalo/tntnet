/* tnt/parser.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

      virtual void pre(char ch)    { }
      virtual bool post(bool ret)  { return ret; }

    public:
      explicit Parser(state_type initialState)
        : state(initialState),
          failedFlag(false)
        { }
      virtual ~Parser() {}

      bool parse(char ch)
      {
        pre(ch);
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
