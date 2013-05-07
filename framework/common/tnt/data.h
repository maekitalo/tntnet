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


#ifndef TNT_DATA_H
#define TNT_DATA_H

#include <iostream>

namespace tnt
{
  class DataChunk
  {
      const char* data;
      unsigned len;

    public:
      DataChunk(const char* data_, const char* end_)
        : data(data_),
          len(end_ - data_)
      { }
      DataChunk(const char* data_, unsigned len_)
        : data(data_),
          len(len_)
      { }

      const char* getData() const  { return data; }
      unsigned getLength() const   { return len; }
  };

  //////////////////////////////////////////////////////////////////////
  // interpretes raw data as a data-structure with chunks
  //   unsigned: number of chunks
  //   count X unsigned: offsets
  //   data
  //
  class DataChunks
  {
      const char* dataObject;

      const unsigned* udata() const
      {
        const char* d = dataObject;
        return reinterpret_cast<const unsigned*>(d);
      }

    public:
      DataChunks(const char* d = 0)
        : dataObject(d)
      { }

      const char* data() const
      { return dataObject; }

      void setData(const char* d)
      { dataObject = d; }

      // number of chunks
      unsigned size() const
      { return (udata()[0] / sizeof(unsigned)) - 1; }

      // offset of n-th chunk from start
      unsigned offset(unsigned n) const
      { return udata()[n] - udata()[0]; }

      // size of nth chunk in bytes
      unsigned size(unsigned n) const
      { return udata()[n + 1] - udata()[n]; }

      // pointer to n-th chunk
      const char* ptr(unsigned n) const
      {
        const char* d = dataObject;
        return d + udata()[n];
      }

      DataChunk operator[] (unsigned n) const
      { return DataChunk(ptr(n), size(n)); }
  };

  inline std::ostream& operator << (std::ostream& out, const DataChunk& c)
  {
    out.write(c.getData(), c.getLength());
    return out;
  }

}

#endif // TNT_DATA_H

