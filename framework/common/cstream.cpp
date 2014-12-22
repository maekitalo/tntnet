/*
 * Copyright (C) 2014 Tommi Maekitalo
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
#include <tnt/cstream.h>
#include <cxxtools/log.h>

log_define("tntnet.cstream")

namespace tnt
{

cstreambuf::~cstreambuf()
{
  log_debug(static_cast<const void*>(this) << " delete " << _chunks.size() << " chunks (dtor)");
  for (size_type n = 0; n < _chunks.size(); ++n)
    delete[] _chunks[n];
}

void cstreambuf::makeEmpty()
{
  log_debug(static_cast<const void*>(this) << " makeEmpty; " << _chunks.size() << " chunks");

  if (_chunks.size() > 0)
  {
    if (_chunks.size() > 1)
    {
      for (size_type n = 1; n < _chunks.size(); ++n)
      {
        log_debug(static_cast<const void*>(this) << " delete chunk " << n);
        delete[] _chunks[n];
      }
      _chunks.resize(1);
    }

    setp(_chunks[0], _chunks[0] + _chunksize);
  }
}

std::streambuf::int_type cstreambuf::overflow(std::streambuf::int_type ch)
{
  char* chunk = new char[_chunksize];
  log_debug(static_cast<const void*>(this) << " new chunk " << static_cast<const void*>(chunk));
  _chunks.push_back(chunk);
  setp(_chunks.back(), _chunks.back() + _chunksize);

  if (ch != traits_type::eof())
    sputc(traits_type::to_char_type(ch));

  return 0;
}

std::streambuf::int_type cstreambuf::underflow()
{
  return traits_type::eof();
}

int cstreambuf::sync()
{
  return 0;
}

void cstreambuf::rollback(size_type n)
{
  if (n == 0)
  {
    makeEmpty();
  }
  else
  {
    size_type c = (n-1) / _chunksize;

    for (size_type cc = c + 1; cc < _chunks.size(); ++cc)
    {
      log_debug(static_cast<const void*>(this) << " delete chunk " << cc);
      delete[] _chunks[cc];
    }

    _chunks.resize(c + 1);

    setp(_chunks[c], _chunks[c] + _chunksize);
    pbump(n % _chunksize);
  }
}

std::string ocstream::str() const
{
  std::string ret;
  ret.reserve(size());
  for (unsigned n = 0; n < chunkcount(); ++n)
    ret.append(chunk(n), chunksize(n));
  return ret;
}

void ocstream::output(std::ostream& out) const
{
  for (unsigned n = 0; n < chunkcount(); ++n)
    out.write(chunk(n), chunksize(n));
}

}
