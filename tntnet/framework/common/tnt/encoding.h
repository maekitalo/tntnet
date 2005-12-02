/* tnt/encoding.h
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

#ifndef TNT_ENCODING_H
#define TNT_ENCODING_H

#include <string>
#include <map>

namespace tnt
{
  class Encoding
  {
      typedef std::map<std::string, unsigned> encodingMapType;
      encodingMapType encodingMap;

    public:
      void parse(const std::string& header);
      /**
       * returns the quality-value in the range 0..10
       */
      unsigned accept(const std::string& encoding) const;
  };
}

#endif // TNT_ENCODING_H
