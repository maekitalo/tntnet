/* tnt/data.h
   Copyright (C) 2003 Tommi Maekitalo

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
      DataChunks(const char* d)
        : dataObject(d)
      { }

      const char* data() const
      { return dataObject; }

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

