/* ./ecpplang.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include "ecpplang.h"

#include <tnt/stringescaper.h>

#include <iostream>
#include <iterator>

void ecpplang::processHtml(const std::string& html)
{
  if (inLang && lang
    || !inLang && nolang)
    data[count] = html;
  ++count;
}

void ecpplang::tokenSplit(bool start)
{
  inLang = start;
}

void ecpplang::print(std::ostream& out) const
{
  for (data_type::const_iterator it = data.begin();
       it != data.end(); ++it)
  {
    std::transform(
      it->second.begin(),
      it->second.end(),
      std::ostream_iterator<const char*>(out),
      stringescaper(false));
    out << '\n';
  }
}
