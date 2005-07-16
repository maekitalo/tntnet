/* jsfilter.h
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

#ifndef TNT_ECPPC_JSFILTER_H
#define TNT_ECPPC_JSFILTER_H

#include <functional>
#include <iosfwd>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Jsfilter
    //
    class Jsfilter : public std::unary_function<void, char>
    {
        std::ostream& out;

        enum state_type 
        {
          state_start,
          state_slash,
          state_space,
          state_comment,
          state_comment_end,
          state_comment1,
          state_string,
          state_stringesc
        };

        state_type state;
        char delim;

      public:
        Jsfilter(std::ostream& out_)
          : out(out_),
            state(state_start)
          { }

        void operator() (char ch);
    };
  }
}

#endif // TNT_ECPPC_JSFILTER_H

