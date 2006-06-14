/* htmlfilter.h
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

#ifndef TNT_ECPPC_HTMLFILTER_H
#define TNT_ECPPC_HTMLFILTER_H

#include <functional>
#include <iosfwd>
#include <string>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Htmlfilter
    //
    class Htmlfilter : public std::unary_function<void, char>
    {
        std::ostream& out;

        enum state_type 
        {
          state_start,
          state_token,
          state_tokenspace,
          state_space0,
          state_space,
          state_data
        };

        state_type state;
        std::string html;

      public:
        Htmlfilter(std::ostream& out_)
          : out(out_),
            state(state_start)
          { }

        void operator() (char ch);
        void flush();
    };
  }
}

#endif // TNT_ECPPC_HTMLFILTER_H

