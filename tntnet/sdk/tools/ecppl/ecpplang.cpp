/* ecpplang.cpp
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

#include "ecpplang.h"

#include <tnt/stringescaper.h>

#include <iostream>
#include <iterator>
#include <algorithm>

void Ecpplang::onHtml(const std::string& html)
{
  if (inLang && lang
    || !inLang && nolang)
    data[count] = html;
  ++count;
}

void Ecpplang::tokenSplit(bool start)
{
  inLang = start;
}

void Ecpplang::print(std::ostream& out) const
{
  for (data_type::const_iterator it = data.begin();
       it != data.end(); ++it)
  {
    std::transform(
      it->second.begin(),
      it->second.end(),
      std::ostream_iterator<const char*>(out),
      tnt::stringescaper(false));
    out << '\n';
  }
}
