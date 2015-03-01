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
#ifndef TNT_CSTREAM
#define TNT_CSTREAM

#include <iostream>
#include <vector>

/// @cond internal

namespace tnt
{

class cstreambuf : public std::streambuf
{
    typedef std::vector<char*> _chunks_type;
    unsigned _chunksize;
    _chunks_type _chunks;

  public:
    typedef _chunks_type::size_type size_type;

    explicit cstreambuf(unsigned chunksize = 32768)
    : _chunksize(chunksize)
    { }

    ~cstreambuf();

    size_type chunkcount() const
    { return _chunks.size(); }

    size_type size() const
    { return _chunks.size() == 0 ? 0
           : (_chunks.size() - 1) * _chunksize + pptr() - _chunks.back(); }

    size_type chunksize(size_type n) const
    {
      return _chunks.size() == 0 ? 0
           : n + 1  < _chunks.size() ? _chunksize
           : n + 1 == _chunks.size() ? static_cast<size_type>(pptr() - _chunks.back())
           : 0;
    }

    const char* chunk(size_type n) const
    { return _chunks[n]; }

    void rollback(size_type n);

    void makeEmpty();

  private:
    std::streambuf::int_type overflow(std::streambuf::int_type ch);
    std::streambuf::int_type underflow();
    int sync();
};

class ocstream : public std::ostream
{
    cstreambuf _streambuf;

  public:
    typedef cstreambuf::size_type size_type;

    explicit ocstream(unsigned chunksize = 32768)
      : std::ostream(0),
        _streambuf(chunksize)
    {
      init(&_streambuf);
    }

    size_type chunkcount() const
    { return _streambuf.chunkcount(); }

    const char* chunk(size_type n) const
    { return _streambuf.chunk(n); }

    size_type chunksize(size_type n) const
    { return _streambuf.chunksize(n); }

    size_type size() const
    { return _streambuf.size(); }

    void rollback(size_type n)
    { _streambuf.rollback(n); }

    void makeEmpty()
    { _streambuf.makeEmpty(); }

    std::string str() const;

    void output(std::ostream& out) const;
};

}

#endif // TNT_CSTREAM
