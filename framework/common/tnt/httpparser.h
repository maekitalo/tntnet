/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
    private:
      size_t _requestSize;

    protected:
      virtual ~RequestSizeMonitor() { }

      void pre(char ch) { }
      bool post(bool ret);

      virtual void requestSizeExceeded();

    public:
      RequestSizeMonitor()
        : _requestSize(0)
        { }
      size_t getCurrentRequestSize() const { return _requestSize; }
      void reset() { _requestSize = 0; }
  };

  class HttpRequest::Parser
    : public tnt::Parser<HttpRequest::Parser, RequestSizeMonitor>
  {
    private:
      HttpRequest& _message;
      Messageheader::Parser _headerParser;

      unsigned _httpCode;

      size_t _bodySize;

      bool state_cmd0(char ch);
      bool state_cmd(char ch);
      bool state_url0(char ch);
      bool state_protocol(char ch);
      bool state_protocol_slash1(char ch);
      bool state_protocol_slash2(char ch);
      bool state_protocol_host(char ch);
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
      Parser(tnt::HttpRequest& message)
        : tnt::Parser<Parser, RequestSizeMonitor>(&Parser::state_cmd0),
          _message(message),
          _headerParser(message.header),
          _httpCode(HTTP_OK)
        { }

      void reset();
  };
}

#endif // TNT_HTTPPARSER_H

