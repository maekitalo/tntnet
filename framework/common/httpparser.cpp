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


#include <tnt/httpparser.h>
#include <tnt/httperror.h>
#include <tnt/httpheader.h>
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>
#include <sstream>
#include <algorithm>

#define SET_STATE(new_state)  state = &Parser::new_state

namespace tnt
{
  namespace
  {
    std::string chartoprint(char ch)
    {
      const static char hex[] = "0123456789abcdef";
      if (std::isprint(ch))
        return std::string(1, '\'') + ch + '\'';
      else
        return std::string("'\\x") + hex[(ch >> 4) & 0xf] + hex[ch & 0xf] + '\'';
    }

    inline bool istokenchar(char ch)
    {
      static const char s[] = "\"(),/:;<=>?@[\\]{}";
      return std::isalpha(ch) || std::binary_search(s, s + sizeof(s) - 1, ch);
    }

    inline bool isHexDigit(char ch)
    {
      return (ch >= '0' && ch <= '9')
          || (ch >= 'A' && ch <= 'F')
          || (ch >= 'a' && ch <= 'f');
    }

    inline unsigned valueOfHexDigit(char ch)
    {
      return ch >= '0' && ch <= '9' ? ch - '0'
           : ch >= 'a' && ch <= 'z' ? ch - 'a' + 10
           : ch >= 'A' && ch <= 'Z' ? ch - 'A' + 10
           : 0;
    }
  }

  log_define("tntnet.httpmessage.parser")

  bool RequestSizeMonitor::post(bool ret)
  {
    if (++requestSize > TntConfig::it().maxRequestSize
      && TntConfig::it().maxRequestSize > 0)
    {
      requestSizeExceeded();
      return true;
    }
    return ret;
  }

  void RequestSizeMonitor::requestSizeExceeded()
  { }

  void HttpRequest::Parser::reset()
  {
    message.clear();
    SET_STATE(state_cmd0);
    httpCode = HTTP_OK;
    failedFlag = false;
    RequestSizeMonitor::reset();
    headerParser.reset();
  }

  bool HttpRequest::Parser::state_cmd0(char ch)
  {
    if (istokenchar(ch))
    {
      message.method[0] = ch;
      message._methodLen = 1;
      SET_STATE(state_cmd);
    }
    else if (ch != ' ' && ch != '\t')
    {
      log_warn("invalid character " << chartoprint(ch) << " in method");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_cmd(char ch)
  {
    if (istokenchar(ch))
    {
      if (message._methodLen >= sizeof(message.method) - 1)
      {
        log_debug("invalid method field; method=" << std::string(message.method, message._methodLen) << ", len=" << message._methodLen);
        throw HttpError(HTTP_BAD_REQUEST, "invalid method field");
      }
      message.method[message._methodLen++] = ch;
    }
    else if (ch == ' ')
    {
      message.method[message._methodLen] = '\0';
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

  bool HttpRequest::Parser::state_url0(char ch)
  {
    if (ch == ' ' || ch == '\t')
    {
    }
    else if (ch == '/')
    {
      message.url.clear();
      message.url.reserve(32);
      message.url += ch;
      SET_STATE(state_url);
    }
    else if (std::isalpha(ch))
    {
      SET_STATE(state_protocol);
    }
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }

    return failedFlag;
  }

  bool HttpRequest::Parser::state_protocol(char ch)
  {
    if (ch == ':')
      SET_STATE(state_protocol_slash1);
    else if (!std::isalpha(ch))
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }

    return failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_slash1(char ch)
  {
    if (ch == '/')
      SET_STATE(state_protocol_slash2);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }

    return failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_slash2(char ch)
  {
    if (ch == '/')
      SET_STATE(state_protocol_host);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }

    return failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_host(char ch)
  {
    if (ch == '/')
    {
      message.url.clear();
      message.url.reserve(32);
      message.url += ch;
      SET_STATE(state_url);
    }
    else if (!std::isalpha(ch)
           && !std::isdigit(ch)
           && ch != '['
           && ch != ']'
           && ch != '.'
           && ch != ':')
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }

    return failedFlag;
  }

  bool HttpRequest::Parser::state_url(char ch)
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
    else if (ch == ' ' || ch == '\t')
    {
      log_debug("url=" << message.url);
      SET_STATE(state_version);
    }
    else if (ch == '%')
    {
      SET_STATE(state_urlesc);
      message.url += ch;
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

  bool HttpRequest::Parser::state_urlesc(char ch)
  {
    if (isHexDigit(ch))
    {
      if (message.url.size() >= 2 && message.url[message.url.size() - 2] == '%')
      {
        unsigned v = (valueOfHexDigit(message.url[message.url.size() - 1]) << 4) | valueOfHexDigit(ch);
        if (v == 0)
          throw HttpError(HTTP_BAD_REQUEST, "invalid value in url");
        message.url[message.url.size() - 2] = static_cast<char>(v);
        message.url.resize(message.url.size() - 1);
        SET_STATE(state_url);
      }
      else
      {
        message.url += ch;
      }
      return false;
    }
    else
    {
      SET_STATE(state_url);
      return state_url(ch);
    }
  }

  bool HttpRequest::Parser::state_qparam(char ch)
  {
    if (ch == ' ' || ch == '\t')
    {
      log_debug("queryString=" << message.queryString);
      SET_STATE(state_version);
    }
    else
      message.queryString += ch;
    return false;
  }

  bool HttpRequest::Parser::state_version(char ch)
  {
    if (ch == '/')
    {
      message.setVersion(0, 0);
      skipWs(&Parser::state_version_major);
    }
    else if (ch == '\r')
    {
      log_warn("invalid character " << chartoprint(ch) << " in version");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_version_major(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor0);
    else if (std::isdigit(ch))
      message.setVersion(message.getMajorVersion() * 10 + (ch - '0'), message.getMinorVersion());
    else if (ch == ' ' || ch == '\t')
      SET_STATE(state_version_major_sp);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-major");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_version_major_sp(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor0);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-major");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_version_minor0(char ch)
  {
    return ch == ' ' || ch == '\t' ? failedFlag
                                   : state_version_minor(ch);
  }

  bool HttpRequest::Parser::state_version_minor(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (ch == ' ' || ch == '\t' || ch == '\r')
      SET_STATE(state_end0);
    else if (std::isdigit(ch))
      message.setVersion(message.getMajorVersion(), message.getMinorVersion() * 10 + (ch - '0'));
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-minor");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_end0(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (ch != ' ' && ch != '\t')
    {
      log_warn("invalid character " << chartoprint(ch) << " in end");
      httpCode = HTTP_BAD_REQUEST;
      failedFlag = true;
    }
    return failedFlag;
  }

  bool HttpRequest::Parser::state_header(char ch)
  {
    if (headerParser.parse(ch))
    {
      if (headerParser.failed())
      {
        httpCode = HTTP_BAD_REQUEST;
        failedFlag = true;
        return true;
      }

      const char* content_length_header = message.getHeader(httpheader::contentLength);
      if (*content_length_header)
      {
        bodySize = 0;
        for (const char* c = content_length_header; *c; ++c)
        {
          if (*c > '9' || *c < '0')
            throw HttpError(HTTP_BAD_REQUEST, "invalid Content-Length");
          bodySize = bodySize * 10 + *c - '0';
        }

        if (TntConfig::it().maxRequestSize > 0
          && getCurrentRequestSize() + bodySize > TntConfig::it().maxRequestSize)
        {
          requestSizeExceeded();
          return true;
        }

        message.contentSize = bodySize;
        if (bodySize == 0)
          return true;
        else
        {
          SET_STATE(state_body);
          message._body.reserve(bodySize);
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool HttpRequest::Parser::state_body(char ch)
  {
    message._body += ch;
    return --bodySize == 0;
  }

  void HttpRequest::Parser::requestSizeExceeded()
  {
    log_warn("max request size " << TntConfig::it().maxRequestSize << " exceeded");
    httpCode = HTTP_REQUEST_ENTITY_TOO_LARGE;
    failedFlag = true;
  }
}
