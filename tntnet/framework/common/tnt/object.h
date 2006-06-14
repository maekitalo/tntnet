/* tnt/object.h
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
 */

#ifndef TNT_OBJECT_H
#define TNT_OBJECT_H

#include <cxxtools/thread.h>

namespace tnt
{
  class Object
  {
      unsigned refs;

      // non-copyable
      Object(const Object&);
      Object& operator=(const Object&);

    protected:
      virtual ~Object()  {}

    public:
      Object()
        : refs(0)
      { }

      virtual unsigned addRef();
      virtual unsigned release();
  };

}

#endif // TNT_OBJECT_H

