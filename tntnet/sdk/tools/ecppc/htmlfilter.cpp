/* htmlfilter.cpp
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

#include "tnt/ecppc/htmlfilter.h"
#include <iostream>
#include <locale>

namespace tnt
{
  namespace ecppc
  {

    ////////////////////////////////////////////////////////////////////////
    // Htmlfilter
    //
    void Htmlfilter::operator() (char ch)
    {
      switch (state)
      {
        case state_start:
          if (ch == '<')
            state = state_token;
          else if (std::isspace(ch, std::locale()))
            state = state_space0;
          out << ch;
          break;

        case state_token:
          if (std::isspace(ch, std::locale()))
          {
            out << ' ';
            state = state_tokenspace;
          }
          else
          {
            if (ch == '>')
              state = state_space0;
            out << ch;
          }
          break;

        case state_tokenspace:
          if (!std::isspace(ch, std::locale()))
          {
            if (ch == '>')
              state = state_space0;
            else
              state = state_token;
            out << ch;
          }
          break;

        case state_space0:
          if (std::isspace(ch, std::locale()))
          {
            out << ch;
            state = state_space;
          }
          // no break!!!

        case state_space:
          if (!std::isspace(ch, std::locale()))
          {
            if (ch == '<')
              // hier lassen wir spaces zwischen Tags weg
              state = state_token;
            else
              // wenn zwischen Tokens was anderes ist, als spaces,
              // dann verändern wir das nicht
              state = state_data;
            out << ch;
          }
          break;

        case state_data:
          if (std::isspace(ch, std::locale()))
            html += ch;
          else if (ch == '<')
          {
            // spaces vor '<' wegwerfen
            html.clear();
            state = state_token;
            out << ch;
          }
          else
          {
            if (!html.empty())
            {
              out << html;
              html.clear();
            }
            out << ch;
          }
          break;
      }
    }

    void Htmlfilter::flush()
    {
      out << html;
      html.clear();
      out.flush();
    }
  }
}
