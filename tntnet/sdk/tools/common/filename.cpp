/* basename.cpp
 * Copyright (C) 2006 Tommi Maekitalo
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

#include "tnt/filename.h"
#include <iostream>

namespace tnt
{
  Filename::Filename(const std::string& filename)
  {
    std::string::size_type pos_slash = filename.find_last_of("\\/");

    if (pos_slash != std::string::npos)
      path = filename.substr(0, ++pos_slash);
    else
      pos_slash = 0;

    std::string::size_type pos_dot = filename.find_last_of('.');

    if (pos_dot != std::string::npos && pos_dot > pos_slash)
    {
      basename = filename.substr(pos_slash, pos_dot - pos_slash);
      ext = filename.substr(pos_dot);
    }
    else
      basename = filename.substr(pos_slash);
  }
}
