/* httpparser.cpp
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <tnt/httpparser.h>
#include <tnt/httperror.h>
#include <tnt/httpheader.h>
#include <tnt/fastctype.h>
#include <cxxtools/log.h>
#include <sstream>
#include <locale>

#define SET_STATE(new_state)  state = &Parser::new_state

namespace tnt
{
  namespace
  {
    std::string chartoprint(char ch)
    {
      const static char hex[] = "0123456789abcdef";
      if (std::isprint(ch, std::locale()))
        return std::string(1, '\'') + ch + '\'';
      else
        return std::string("'\\x") + hex[(ch >> 4) & 0xf] + hex[ch & 0xf] + '\'';
    }

    inline bool istokenchar(char ch)
    {
      static const char s[] = "\"(),/:;<=>?@[\\]{}";
      return myisalpha(ch) || std::binary_search(s, s + sizeof(s) - 1, ch);
    }
  }

  log_define("tntnet.httpmessage.parser")

  void HttpMessage::Parser::reset()
  {
    message.clear();
    SET_STATE(state_cmd0);
    httpCode = HTTP_OK;
    failedFlag = false;
    requestSize = 0;
    headerParser.reset();
  }

  bool HttpMessage::Parser::state_cmd0(char ch)
  {
    if (istokenchar(ch))
    {
      message.method = ch;
      SET_STATE(state_cmd);
    }
    else if (!myisspace(ch))
    {
      log_warn("invalid character " << chartoprint(ch) << " in method");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_cmd(char ch)
  {
    if (istokenchar(ch))
      message.method += ch;
    else if (ch == ' ')
    {
      log_debug("method=" << message.method);
      SET_STATE(state_url0);
    }
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in method");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_url0(char ch)
  {
    if (!myisspace(ch))
    {
      if (ch > ' ')
      {
        message.url = ch;
        SET_STATE(state_url);
      }
      else
      {
        log_warn("invalid character " << chartoprint(ch) << " in url");
        httpCode = HTTP_BAD_REQUEST;
        failedFlag = true;
      }
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_url(char ch)
  {
    if (ch == '?')
    {
      log_debug("url=" << message.url);
      SET_STATE(state_qparam);
    }
    else if (ch == '\r')
    {
      log_debug("url=" << message.url);
      SET_STATE(state_end0);
    }
    else if (ch == '\n')
    {
      log_debug("url=" << message.url);
      SET_STATE(state_header);
    }
    else if (ch == ' ')
    {
      log_debug("url=" << message.url);
      SET_STATE(state_version);
    }
    else if (ch > ' ')
      message.url += ch;
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_qparam(char ch)
  {
    if (myisspace(ch))
    {
      log_debug("queryString=" << message.queryString);
      SET_STATE(state_version);
    }
    else
      message.queryString += ch;
    return false;
  }

  bool HttpMessage::Parser::state_version(char ch)
  {
    if (ch == '/')
    {
      message.majorVersion = 0;
      message.minorVersion = 0;
      SET_STATE(state_version_major);
    }
    else if (ch == '\r')
    {
      log_warn("invalid character " << chartoprint(ch) << " in version");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_version_major(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor);
    else if (myisdigit(ch))
      message.majorVersion = message.majorVersion * 10 + (ch - '0');
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-major");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_version_minor(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (myisspace(ch))
      SET_STATE(state_end0);
    else if (myisdigit(ch))
      message.minorVersion = message.minorVersion * 10 + (ch - '0');
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-minor");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_end0(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (!myisspace(ch))
    {
      log_warn("invalid character " << chartoprint(ch) << " in end");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpMessage::Parser::state_header(char ch)
  {
    if (headerParser.parse(ch))
    {
      if (headerParser.failed())
      {
        httpCode = HTTP_BAD_REQUEST;
        failedFlag = true;
        return true;
      }

      std::string content_length_header = message.getHeader(httpheader::contentLength);
      if (!content_length_header.empty())
      {
        std::istringstream valuestream(content_length_header);
        valuestream >> bodySize;
        if (!valuestream)
          throw HttpError("400 missing Content-Length");

        message.contentSize = bodySize;
        if (bodySize == 0)
          return true;
        else
        {
          SET_STATE(state_body);
          message.body.reserve(bodySize);
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool HttpMessage::Parser::state_body(char ch)
  {
    message.body += ch;
    return --bodySize == 0;
  }

  bool HttpMessage::Parser::post(bool ret)
  {
    if (++requestSize > maxRequestSize && maxRequestSize > 0)
    {
      log_warn("max request size " << maxRequestSize << " exceeded");
      httpCode = HTTP_REQUEST_ENTITY_TOO_LARGE;
      failedFlag = true;
      return true;
    }

    return ret;
  }
}
