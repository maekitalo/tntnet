/* tnt/pointer.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNT_POINTER_H
#define TNT_POINTER_H

namespace tnt
{
  /// smart pointer for objects with own reference-counting through
  /// addRef() and release()-methods.
  template <typename data_type>
  class Pointer
  {
      data_type* ptr;

    public:
      Pointer(data_type* p = 0)
        : ptr(p)
      {
        if (ptr)
          ptr->addRef();
      }

      Pointer(const Pointer& p)
        : ptr(p.ptr)
      {
        if (ptr)
          ptr->addRef();
      }

      ~Pointer()
      {
        if (ptr)
          ptr->release();
      }

      Pointer& operator= (const Pointer& s)
      {
        if (ptr)
          ptr->release();
        ptr = s.ptr;
        if (ptr)
          ptr->addRef();
        return *this;
      }

      const data_type* getPtr() const      { return ptr; }
      data_type* getPtr()                  { return ptr; }
      operator const data_type* () const   { return ptr; }
      const data_type* operator-> () const { return ptr; }
      data_type* operator-> ()             { return ptr; }
  };
}

#endif // TNT_POINTER_H

