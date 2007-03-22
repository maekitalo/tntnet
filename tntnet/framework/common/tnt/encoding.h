/* tnt/encoding.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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
