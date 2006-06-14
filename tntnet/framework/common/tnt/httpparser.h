/* tnt/httpparser.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#ifndef TNT_HTTPPARSER_H
#define TNT_HTTPPARSER_H

#include <tnt/httpmessage.h>
#include <tnt/http.h>
#include <tnt/parser.h>
#include <tnt/messageheaderparser.h>

namespace tnt
{
  class HttpMessage::Parser : public tnt::Parser<HttpMessage::Parser>
  {
      HttpMessage& message;
      Messageheader::Parser headerParser;

      unsigned httpCode;

      size_t requestSize;
      size_t bodySize;

      bool state_cmd0(char ch);
      bool state_cmd(char ch);
      bool state_url0(char ch);
      bool state_url(char ch);
      bool state_qparam(char ch);
      bool state_version(char ch);
      bool state_version_major(char ch);
      bool state_version_minor(char ch);
      bool state_end0(char ch);
      bool state_header(char ch);
      bool state_body(char ch);

      bool post(bool ret);

    public:
      Parser(tnt::HttpMessage& message_)
        : tnt::Parser<Parser>(&Parser::state_cmd0),
          message(message_),
          headerParser(message_.header),
          httpCode(HTTP_OK),
          requestSize(0)
        { }

      void reset();
  };
}

#endif // TNT_HTTPPARSER_H
