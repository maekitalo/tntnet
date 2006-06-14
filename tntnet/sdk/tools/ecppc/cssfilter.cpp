/* cssfilter.cpp
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "tnt/ecppc/cssfilter.h"
#include <iostream>
#include <locale>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Cssfilter
    //
    void Cssfilter::operator() (char ch)
    {
      switch (state)
      {
        case state_start:
          if (ch == '/')
            state = state_slash;
          else if (std::isspace(ch, std::locale()))
          {
            out << ch;
            state = state_space;
          }
          else
            out << ch;
          break;

        case state_slash:
          if (ch == '*')
          {
            out << ' ';
            state = state_comment;
          }
          else
          {
            out << '/' << ch;
            state = state_start;
          }
          break;

        case state_space:
          if (ch == '/')
            state = state_slash;
          else if (!std::isspace(ch, std::locale()))
          {
            out << ch;
            state = state_start;
          }
          break;

        case state_comment:
          if (ch == '*')
            state = state_comment_end;
          break;

        case state_comment_end:
          if (ch == '/')
            state = state_start;
          else if (ch != '*')
            state = state_comment;
          break;
      }
    }
  }
}
