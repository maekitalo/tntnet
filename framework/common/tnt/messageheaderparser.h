/*
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef TNT_MESSAGEHEADERPARSER_H
#define TNT_MESSAGEHEADERPARSER_H

#include <tnt/messageheader.h>
#include <tnt/parser.h>

namespace tnt
{
  class Messageheader::Parser : public tnt::Parser<Messageheader::Parser>
  {
      Messageheader& _header;
      char* _headerdataPtr;
      char* _fieldnamePtr;
      char* _fieldbodyPtr;

      bool state_0(char ch);
      bool state_cr(char ch);
      bool state_fieldname(char ch);
      bool state_fieldnamespace(char ch);
      bool state_fieldbody0(char ch);
      bool state_fieldbody(char ch);
      bool state_fieldbody_cr(char ch);
      bool state_fieldbody_crlf(char ch);
      bool state_end_cr(char ch);

      void checkHeaderspace(unsigned chars) const;

    public:
      explicit Parser(Messageheader& header)
        : tnt::Parser<Parser>(&Parser::state_0),
          _header(header),
          _headerdataPtr(header._rawdata),
          _fieldnamePtr(0),
          _fieldbodyPtr(0)
          { }

      void reset();
  };
}

#endif // TNT_MESSAGEHEADERPARSER_H

