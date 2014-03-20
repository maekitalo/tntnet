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

#define SET_STATE(new_state) _state = &Parser::new_state

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
    if (++_requestSize > TntConfig::it().maxRequestSize
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
    _message.clear();
    SET_STATE(state_cmd0);
    _httpCode = HTTP_OK;
    _failedFlag = false;
    RequestSizeMonitor::reset();
    _headerParser.reset();
  }

  bool HttpRequest::Parser::state_cmd0(char ch)
  {
    if (istokenchar(ch))
    {
      _message._method[0] = ch;
      _message._methodLen = 1;
      SET_STATE(state_cmd);
    }
    else if (ch != ' ' && ch != '\t')
    {
      log_warn("invalid character " << chartoprint(ch) << " in method");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_cmd(char ch)
  {
    if (istokenchar(ch))
    {
      if (_message._methodLen >= sizeof(_message._method) - 1)
      {
        log_debug("invalid method field; method=" << std::string(_message._method, _message._methodLen) << ", len=" << _message._methodLen);
        throw HttpError(HTTP_BAD_REQUEST, "invalid method field");
      }
      _message._method[_message._methodLen++] = ch;
    }
    else if (ch == ' ')
    {
      _message._method[_message._methodLen] = '\0';
      log_debug("method=" << _message._method);
      SET_STATE(state_url0);
    }
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in method");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_url0(char ch)
  {
    if (ch == ' ' || ch == '\t')
    {
    }
    else if (ch == '/')
    {
      _message._url.clear();
      _message._url.reserve(32);
      _message._url += ch;
      SET_STATE(state_url);
    }
    else if (std::isalpha(ch))
    {
      SET_STATE(state_protocol);
    }
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }

    return _failedFlag;
  }

  bool HttpRequest::Parser::state_protocol(char ch)
  {
    if (ch == ':')
      SET_STATE(state_protocol_slash1);
    else if (!std::isalpha(ch))
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }

    return _failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_slash1(char ch)
  {
    if (ch == '/')
      SET_STATE(state_protocol_slash2);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }

    return _failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_slash2(char ch)
  {
    if (ch == '/')
      SET_STATE(state_protocol_host);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }

    return _failedFlag;
  }

  bool HttpRequest::Parser::state_protocol_host(char ch)
  {
    if (ch == '/')
    {
      _message._url.clear();
      _message._url.reserve(32);
      _message._url += ch;
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
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }

    return _failedFlag;
  }

  bool HttpRequest::Parser::state_url(char ch)
  {
    if (ch == '?')
    {
      log_debug("url=" << _message._url);
      SET_STATE(state_qparam);
    }
    else if (ch == '\r')
    {
      log_debug("url=" << _message._url);
      SET_STATE(state_end0);
    }
    else if (ch == '\n')
    {
      log_debug("url=" << _message._url);
      SET_STATE(state_header);
    }
    else if (ch == ' ' || ch == '\t')
    {
      log_debug("url=" << _message._url);
      SET_STATE(state_version);
    }
    else if (ch == '%')
    {
      SET_STATE(state_urlesc);
      _message._url += ch;
    }
    else if (ch > ' ')
      _message._url += ch;
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in url");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_urlesc(char ch)
  {
    if (isHexDigit(ch))
    {
      if (_message._url.size() >= 2 && _message._url[_message._url.size() - 2] == '%')
      {
        unsigned v = (valueOfHexDigit(_message._url[_message._url.size() - 1]) << 4) | valueOfHexDigit(ch);
        if (v == 0)
          throw HttpError(HTTP_BAD_REQUEST, "invalid value in url");
        _message._url[_message._url.size() - 2] = static_cast<char>(v);
        _message._url.resize(_message._url.size() - 1);
        SET_STATE(state_url);
      }
      else
      {
        _message._url += ch;
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
      log_debug("queryString=" << _message._queryString);
      SET_STATE(state_version);
    }
    else
      _message._queryString += ch;
    return false;
  }

  bool HttpRequest::Parser::state_version(char ch)
  {
    if (ch == '/')
    {
      _message.setVersion(0, 0);
      skipWs(&Parser::state_version_major);
    }
    else if (ch == '\r')
    {
      log_warn("invalid character " << chartoprint(ch) << " in version");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_version_major(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor0);
    else if (std::isdigit(ch))
      _message.setVersion(_message.getMajorVersion() * 10 + (ch - '0'), _message.getMinorVersion());
    else if (ch == ' ' || ch == '\t')
      SET_STATE(state_version_major_sp);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-major");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_version_major_sp(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor0);
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-major");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_version_minor0(char ch)
  {
    return ch == ' ' || ch == '\t' ? _failedFlag
                                   : state_version_minor(ch);
  }

  bool HttpRequest::Parser::state_version_minor(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (ch == ' ' || ch == '\t' || ch == '\r')
      SET_STATE(state_end0);
    else if (std::isdigit(ch))
      _message.setVersion(_message.getMajorVersion(), _message.getMinorVersion() * 10 + (ch - '0'));
    else
    {
      log_warn("invalid character " << chartoprint(ch) << " in version-minor");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_end0(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (ch != ' ' && ch != '\t')
    {
      log_warn("invalid character " << chartoprint(ch) << " in end");
      _httpCode = HTTP_BAD_REQUEST;
      _failedFlag = true;
    }
    return _failedFlag;
  }

  bool HttpRequest::Parser::state_header(char ch)
  {
    if (_headerParser.parse(ch))
    {
      if (_headerParser.failed())
      {
        _httpCode = HTTP_BAD_REQUEST;
        _failedFlag = true;
        return true;
      }

      const char* content_length_header = _message.getHeader(httpheader::contentLength);
      if (*content_length_header)
      {
        _bodySize = 0;
        for (const char* c = content_length_header; *c; ++c)
        {
          if (*c > '9' || *c < '0')
            throw HttpError(HTTP_BAD_REQUEST, "invalid Content-Length");
          _bodySize = _bodySize * 10 + *c - '0';
        }

        if (TntConfig::it().maxRequestSize > 0
          && getCurrentRequestSize() + _bodySize > TntConfig::it().maxRequestSize)
        {
          requestSizeExceeded();
          return true;
        }

        _message._contentSize = _bodySize;
        if (_bodySize == 0)
          return true;
        else
        {
          SET_STATE(state_body);
          _message._body.reserve(_bodySize);
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool HttpRequest::Parser::state_body(char ch)
  {
    _message._body += ch;
    return --_bodySize == 0;
  }

  void HttpRequest::Parser::requestSizeExceeded()
  {
    log_warn("max request size " << TntConfig::it().maxRequestSize << " exceeded");
    _httpCode = HTTP_REQUEST_ENTITY_TOO_LARGE;
    _failedFlag = true;
  }
}

