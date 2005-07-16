/* tnt/messageheader.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef TNT_MESSAGEHEADER_H
#define TNT_MESSAGEHEADER_H

#include <string>
#include <map>

namespace tnt
{
  /// Standard-message-header like rfc822
  class Messageheader : public std::multimap<std::string, std::string>
  {
    public:
      class Parser;
      friend class Parser;

    protected:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

      virtual return_type onField(const std::string& name, const std::string& value);

    public:
      virtual ~Messageheader()   { }

      void parse(std::istream& in, size_t maxHeaderSize = 0);
  };

  std::istream& operator>> (std::istream& in, Messageheader& data);

}

#endif // TNT_MESSAGEHEADER_H

