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


#include <tnt/mimedb.h>
#include <tnt/util.h>
#include <cctype>
#include <iostream>
#include <fstream>
#include <locale>
#include <stdexcept>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.mime")

  void MimeDb::read(const std::string& mimefile)
  {
    std::ifstream in(mimefile.c_str());
    read(in);
  }

  void MimeDb::read(const char* mimefile)
  {
    std::ifstream in(mimefile);
    read(in);
  }

  void MimeDb::read(std::istream& in)
  {
    enum state_type {
      state_0,
      state_comment,
      state_mime,
      state_ext0,
      state_ext
    } state = state_0;

    std::string mime;
    std::string ext;

    std::streambuf* buf = in.rdbuf();
    while (buf->sgetc() != std::ios::traits_type::eof())
    {
      char ch = buf->sbumpc();
      switch (state)
      {
        case state_0:
          if ((ch >= 'a' && ch <= 'z')
           || (ch >= 'A' && ch <= 'Z'))
          {
            mime = ch;
            state = state_mime;
          }
          else if (ch == '#')
            state = state_comment;
          else if (!std::isspace(ch))
            throwRuntimeError("parse error in mimedb");
          break;

        case state_comment:
          if (ch == '\n')
            state = state_0;
          break;

        case state_mime:
          if (ch == '\n')
            state = state_0;
          else if (std::isspace(ch))
            state = state_ext0;
          else
            mime += ch;
          break;

        case state_ext0:
          if (ch == '\n')
            state = state_0;
          else if (ch == '.')
          {
            ext.clear();
            state = state_ext;
          }
          else if (!std::isspace(ch))
          {
            ext = ch;
            state = state_ext;
          }
          break;

        case state_ext:
          if (std::isspace(ch))
          {
            log_debug(ext << " => " << mime);
            _mimeDb.insert(MimeDbType::value_type(ext, mime));
            state = ch == '\n' ? state_0 : state_ext0;
          }
          else
            ext += ch;
      }
    }
  }

  void MimeDb::addType(const std::string& ext, const std::string& mimeType)
  {
    if (ext.size() > 0 && ext.at(0) == '.')
      _mimeDb.insert(MimeDbType::value_type(ext.substr(1), mimeType));
    else
      _mimeDb.insert(MimeDbType::value_type(ext, mimeType));
  }

  std::string MimeDb::getMimetype(const std::string& fname) const
  {
    log_debug("get mimetype for \"" << fname << '"');

    std::string ext;

    std::string::size_type p = fname.rfind('.');
    if (p != std::string::npos)
      ext = fname.substr(p + 1);
    else
      ext = fname;

    log_debug("ext=" << ext);
    MimeDbType::const_iterator it = _mimeDb.find(ext);
    if (it == _mimeDb.end())
    {
      log_debug("no mimetype found for ext \"" << ext << '"');
      return std::string();
    }
    else
    {
      log_debug("mimetype for ext \"" << ext << "\": " << it->second);
      return it->second;
    }
  }
}

