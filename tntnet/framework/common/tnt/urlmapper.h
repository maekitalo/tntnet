/* tnt/urlmapper.h
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

#ifndef TNT_URLMAPPER_H
#define TNT_URLMAPPER_H

#include <tnt/component.h>

namespace tnt
{
  // map compUrl to compident
  class urlmapper
  {
    public:
      virtual ~urlmapper()  { }
      virtual compident mapComp(const std::string& compUrl) const = 0;
  };
}

#endif // TNT_URLMAPPER_H

