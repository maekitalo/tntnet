/* tnt/messageheader.h
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

#ifndef TNT_MESSAGEHEADER_H
#define TNT_MESSAGEHEADER_H

#include <string>
#include <map>
#include <iosfwd>

namespace tnt
{
  /// Standard-message-header like rfc822
  class messageheader : public std::multimap<std::string, std::string>
  {
    protected:
      enum return_type
      {
        OK,
        FAIL,
        END
      };

    protected:
      virtual return_type onField(const std::string& name, const std::string& value);

    public:
      messageheader()
      { }
      virtual ~messageheader()
      { }

      void parse(std::istream& in);
  };

  std::istream& operator>> (std::istream& in, messageheader& data);
}

#endif // TNT_MESSAGEHEADER_H

