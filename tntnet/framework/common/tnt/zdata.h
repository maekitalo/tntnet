/* tnt/zdata.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef TNT_ZDATA_H
#define TNT_ZDATA_H

namespace tnt
{
  class zdata
  {
      const char* zptr;
      const unsigned zdata_len;
      const unsigned data_len;

      unsigned refs;
      char* data;

    public:
      zdata(const char* zptr_, unsigned zdata_len_, unsigned data_len_)
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

