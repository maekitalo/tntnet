/* tnt/messageheaderparser.h
   Copyright (C) 2003 Tommi Mae¤kitalo

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

#ifndef TNT_MESSAGEHEADERPARSER_H
#define TNT_MESSAGEHEADERPARSER_H

#include <tnt/messageheader.h>
#include <tnt/parser.h>

namespace tnt
{
  class messageheader::parser : public tnt::parser<messageheader::parser>
  {
      messageheader& header;
      std::string fieldname;
      std::string fieldbody;

      bool state_0(char ch);
      bool state_cr(char ch);
      bool state_fieldname(char ch);
      bool state_fieldnamespace(char ch);
      bool state_fieldbody0(char ch);
      bool state_fieldbody(char ch);
      bool state_fieldbody_cr(char ch);
      bool state_fieldbody_crlf(char ch);
      bool state_end_cr(char ch);

    public:
      parser(messageheader& header_)
        : tnt::parser<parser>(&parser::state_0),
          header(header_)
          { }

      void reset();
  };
}

#endif // TNT_MESSAGEHEADERPARSER_H

