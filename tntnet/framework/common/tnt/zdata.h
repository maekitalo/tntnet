/* tnt/zdata.h
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_ZDATA_H
#define TNT_ZDATA_H

namespace tnt
{
  class Zdata
  {
      const char* zptr;
      const unsigned zdata_len;
      const unsigned data_len;

      unsigned refs;
      char* data;

    public:
      Zdata(const char* zptr_, unsigned zdata_len_, unsigned data_len_)
        : zptr(zptr_),
          zdata_len(zdata_len_),
          data_len(data_len_),
          refs(0),
          data(0)
      { }

      void addRef();
      void release();

      operator const char* () const      { return data; }
  };
}

#endif // TNT_ZDATA_H

