/* tnt/objecttemplate.h
   Copyright (C) 2005 Tommi Maekitalo

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

#ifndef TNT_OBJECTTEMPLATE_H
#define TNT_OBJECTTEMPLATE_H

#include <tnt/object.h>

namespace tnt
{
  template <typename data_type>
  class objectTemplate : public object
  {
      data_type data;

    public:
      typedef data_type& reference;
      typedef const data_type& const_reference;

      objectTemplate() { }

      template <typename t1>
      objectTemplate(t1 p1)
        : data(p1) { }

      template <typename t1, typename t2>
      objectTemplate(t1 p1, t2 p2)
        : data(p1, p2) { }

      template <typename t1, typename t2, typename t3>
      objectTemplate(t1 p1, t2 p2, t3 p3)
        : data(p1, p2, p3) { }

      template <typename t1, typename t2, typename t3, typename t4>
      objectTemplate(t1 p1, t2 p2, t3 p3, t4 p4)
        : data(p1, p2, p3, p4) { }

      // explicit accessors
      const data_type& getData() const    { return data; }
      data_type& getData()                { return data; }
      void setData(const data_type& d)    { data = d; }

      // implicit accessors
      operator reference()                { return data; }
      operator const reference() const    { return data; }
      reference operator*()               { return data; }
      const reference operator*() const   { return data; }
      objectTemplate& operator= (const data_type& d)  { data = d; return *this; }
  };
}

#endif // TNT_OBJECTTEMPLATE_H

