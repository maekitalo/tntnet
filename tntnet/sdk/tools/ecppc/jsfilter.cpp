/* jsfilter.cpp
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

#include "tnt/ecppc/jsfilter.h"
#include <iostream>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // jsfilter
    //
    void jsfilter::operator() (char ch)
    {
      switch (state)
      {
        case state_start:
          if (ch == '/')
            state = state_slash;
          else
          {
            if (std::isspace(ch))
            {
              state = state_space;
            }
            else if (ch == '"' || ch == '\'')
            {
              delim = ch;
              state = state_string;
            }
            out << ch;
          }
          break;

        case state_slash:
          if (ch == '*')
          {
            out << ' ';
            state = state_comment;
          }
          else if (ch == '/')
          {
            out << ' ';
            state = state_comment1;
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
          else if (!std::isspace(ch))
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
            state = state_space;
          else if (ch != '*')
            state = state_comment;
          break;

        case state_comment1:
          if (ch == '\n')
            state = state_space;
          break;

        case state_string:
          if (ch == '\\')
            state = state_stringesc;
          else
          {
            out << ch;
            if (ch == delim)
              state = state_start;
          }
          break;

        case state_stringesc:
          out << ch;
          break;
      }
    }
  }
}
