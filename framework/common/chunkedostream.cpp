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


#include <tnt/chunkedostream.h>

namespace tnt
{
  int ChunkedWriter::sync()
  {
    static const char hex[] = "0123456789ABCDEF";
    unsigned size = pptr() - pbase();
    if (size > 0)
    {
      unsigned b = 0;
      while ((static_cast<unsigned>(0xf << b)) < size)
        b += 4;
      while (b > 0)
      {
        _obuf->sputc(hex[(size >> b) & 0xf]);
        b -= 4;
      }
      _obuf->sputc(hex[size & 0xf]);
      _obuf->sputc('\r');
      _obuf->sputc('\n');
      _obuf->sputn(pbase(), size);
      _obuf->sputc('\r');
      _obuf->sputc('\n');
      setp(_buffer, _buffer + _bufsize);
    }

    return 0;
  }

  ChunkedWriter::int_type ChunkedWriter::overflow(int_type ch)
  {
    if (_buffer == 0)
    {
      _buffer = new char[_bufsize];
      setp(_buffer, _buffer + _bufsize);
    }
    else
    {
      sync();
    }

    if (ch != traits_type::eof())
      sputc(ch);

    return ch;
  }

  ChunkedWriter::int_type ChunkedWriter::underflow()
    { return traits_type::eof(); }

  void ChunkedWriter::finish()
  {
    sync();
    _obuf->sputn("0\r\n", 3);
  }
}

