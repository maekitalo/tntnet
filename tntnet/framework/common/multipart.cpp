/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#include <tnt/multipart.h>
#include <tnt/httpheader.h>
#include <tnt/util.h>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <tnt/stringlessignorecase.h>

namespace
{
  template <typename iterator>
    class iterator_streambuf : public std::streambuf
  {
    public:
      typedef unsigned long streamsize;

    private:
      iterator& begin;
      iterator end;
      char_type buffer[2];

    protected:
      std::streambuf::int_type overflow(std::streambuf::int_type ch)
        { return traits_type::eof(); }
      int sync()
        {
          if (gptr() == buffer + 1)
            ++begin;
          setg(0, 0, 0);
          return 0;
        }
      std::streambuf::int_type underflow()
        {
          if (begin == end)
            return traits_type::eof();

          if (gptr() == buffer + 1)
            ++begin;

          buffer[0] = *begin;
          setg(buffer, buffer, buffer + 1);
          return buffer[0];
        }

    public:
      iterator_streambuf(iterator& b, iterator e)
        : begin(b),
          end(e)
        { }

  };
}

namespace tnt
{
  Partheader::return_type Partheader::onField(const char* name,
    const char* value)
  {
    if (tnt::StringCompareIgnoreCase<const char*>(name, "Content-Disposition:") == 0)
    {
      std::istringstream in(value);
      in >> cd;
      if (!in)
        return FAIL;
    }

    return Messageheader::onField(name, value);
  }

  Part::Part(const_iterator b, const_iterator e)
  {
    iterator_streambuf<const_iterator> buf(b, e);
    std::istream in(&buf);
    in >> header;
    if (!in)
      throwRuntimeError("error in parsing message-header");
    in.sync();

    bodyBegin = b;
    bodyEnd = e;
  }

  std::string Part::getHeader(const std::string& key) const
  {
    Messageheader::const_iterator it = header.find(key);
    if (it != header.end())
      return it->second;
    return std::string();
  }

  std::string Partheader::getMimetype() const
  {
    const_iterator it = find(httpheader::contentType);
    return it == end() ? std::string() : it->second;
  }

  void Multipart::set(const std::string& boundary, const std::string& b)
  {
    body = b;

    std::string::size_type bpos = body.find(boundary);
    while (bpos != std::string::npos)
    {
      bpos += boundary.size();
      if (body[bpos] == '\r')
        ++bpos;
      if (body[bpos] == '\n')
        ++bpos;

      std::string::size_type bend = body.find(boundary, bpos);
      if (bend == std::string::npos)
        return;

      std::string::size_type nbegin = bend;

      if (body[bend-1] == '-')
        --bend;
      if (body[bend-1] == '-')
        --bend;
      if (body[bend-1] == '\n')
        --bend;
      if (body[bend-1] == '\r')
        --bend;

      parts.push_back(part_type(body.begin() + bpos, body.begin() + bend));
      bpos = nbegin;
    }
  }

  Multipart::const_iterator Multipart::find(const std::string& part_name,
        Multipart::const_iterator start) const
  {
    for (const_iterator it = start; it != end(); ++it)
      if (it->getName() == part_name)
        return it;

    return end();
  }
}
