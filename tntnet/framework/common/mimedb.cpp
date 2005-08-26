/* mimedb.cpp
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

#include "tnt/mimedb.h"
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
        if (ch >= 'a' && ch <= 'z'
         || ch >= 'A' && ch <= 'Z')
        {
          mime = ch;
          state = state_mime;
        }
        else if (ch == '#')
          state = state_comment;
        else if (!std::isspace(ch))
          throw std::runtime_error("parse error in mimedb");
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
          mimeDb.insert(mimedb_type::value_type(ext, mime));
          state = ch == '\n' ? state_0 : state_ext0;
        }
        else
          ext += ch;
    }
  }
}

std::string MimeDb::getMimetype(const std::string& fname) const
{
  std::string ext;

  std::string::size_type p = fname.rfind('.');
  if (p != std::string::npos)
    ext = fname.substr(p + 1);
  else
    ext = fname;

  const_iterator it = find(ext);
  return it == end() ? std::string() : it->second;
}

}
