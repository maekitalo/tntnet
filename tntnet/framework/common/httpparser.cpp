/* httpparser.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#include <tnt/httpparser.h>
#include <tnt/httperror.h>
#include <cxxtools/log.h>
#include <sstream>

#define SET_STATE(new_state)  state = &parser::new_state

namespace tnt
{
  log_define("tntnet.httpmessage.parser");

  void httpMessage::parser::clear()
  {
    message.clear();
    SET_STATE(state_cmd0);
    failed_flag = false;
  }

  bool httpMessage::parser::state_cmd0(char ch)
  {
    if (!std::isspace(ch))
    {
      message.method = ch;
      SET_STATE(state_cmd);
    }
    return false;
  }

  bool httpMessage::parser::state_cmd(char ch)
  {
    if (std::isspace(ch))
      SET_STATE(state_url0);
    else
      message.method += ch;
    return false;
  }

  bool httpMessage::parser::state_url0(char ch)
  {
    if (!std::isspace(ch))
    {
      message.url = ch;
      SET_STATE(state_url);
    }
    return false;
  }

  bool httpMessage::parser::state_url(char ch)
  {
    log_debug("httpMessage::parser::state_url(" << ch << ')');
    if (ch == '?')
      SET_STATE(state_qparam);
    else if (std::isspace(ch))
      SET_STATE(state_version);
    else
      message.url += ch;
    return false;
  }

  bool httpMessage::parser::state_qparam(char ch)
  {
    log_debug("httpMessage::parser::state_qparam(" << ch << ')');
    if (std::isspace(ch))
      SET_STATE(state_version);
    else
      message.query_string += ch;
    return false;
  }

  bool httpMessage::parser::state_version(char ch)
  {
    if (ch == '/')
      SET_STATE(state_version_major);
    else if (ch == '\r')
      failed_flag = true;
    return failed_flag;
  }

  bool httpMessage::parser::state_version_major(char ch)
  {
    if (ch == '.')
      SET_STATE(state_version_minor);
    else if (std::isdigit(ch))
      message.major_version = message.major_version * 10 + (ch - '0');
    else
      failed_flag = true;
    return failed_flag;
  }

  bool httpMessage::parser::state_version_minor(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (std::isspace(ch))
      SET_STATE(state_end0);
    else if (std::isdigit(ch))
      message.minor_version = message.minor_version * 10 + (ch - '0');
    else
      failed_flag = true;
    return failed_flag;
  }

  bool httpMessage::parser::state_end0(char ch)
  {
    if (ch == '\n')
      SET_STATE(state_header);
    else if (!std::isspace(ch))
      failed_flag = true;
    return failed_flag;
  }

  bool httpMessage::parser::state_header(char ch)
  {
    if (headerParser.parse(ch))
    {
      std::string content_length_header = message.getHeader(Content_Length);
      if (!content_length_header.empty())
      {
        std::istringstream valuestream(content_length_header);
        valuestream >> content_size;
        if (!valuestream)
          throw httpError("400 missing Content-Length");

        if (maxBodySize > 0 && content_size > maxBodySize)
        {
          log_warn("max body size " << maxBodySize << " exceeded");
          failed_flag = true;
          return true;
        }
        else
        {
          message.content_size = content_size;
          if (content_size == 0)
            return true;
          else
          {
            SET_STATE(state_body);
            return false;
          }
        }
      }

      return true;
    }

    return false;
  }

  bool httpMessage::parser::state_body(char ch)
  {
    message.body += ch;
    return --content_size == 0;
  }
}
