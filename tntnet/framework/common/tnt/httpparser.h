/* tnt/httpparser.h
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


#ifndef TNT_HTTPPARSER_H
#define TNT_HTTPPARSER_H

#include <tnt/httprequest.h>
#include <tnt/http.h>
#include <tnt/parser.h>
#include <tnt/messageheaderparser.h>

namespace tnt
{
  class RequestSizeMonitor
  {
      size_t requestSize;

    protected:
      void pre(char ch)    { }
      bool post(bool ret);

      virtual void requestSizeExceeded();

    public:
      RequestSizeMonitor()
        : requestSize(0)
        { }
      size_t getCurrentRequestSize() const  { return requestSize; }
      void reset()  { requestSize = 0; }
  };

  class HttpRequest::Parser
    : public tnt::Parser<HttpRequest::Parser, RequestSizeMonitor>
  {
      HttpRequest& message;
      Messageheader::Parser headerParser;

      unsigned httpCode;

      size_t bodySize;

      bool state_cmd0(char ch);
      bool state_cmd(char ch);
      bool state_url0(char ch);
      bool state_url(char ch);
      bool state_urlesc(char ch);
      bool state_qparam(char ch);
      bool state_version(char ch);
      bool state_version_major(char ch);
      bool state_version_major_sp(char ch);
      bool state_version_minor0(char ch);
      bool state_version_minor(char ch);
      bool state_end0(char ch);
      bool state_header(char ch);
      bool state_body(char ch);

    protected:
      virtual void requestSizeExceeded();

    public:
      Parser(tnt::HttpRequest& message_)
        : tnt::Parser<Parser, RequestSizeMonitor>(&Parser::state_cmd0),
          message(message_),
          headerParser(message_.header),
          httpCode(HTTP_OK)
        { }

      void reset();
  };
}

#endif // TNT_HTTPPARSER_H
