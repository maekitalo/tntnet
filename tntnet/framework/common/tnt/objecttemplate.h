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

      template <typename t0>
      objectTemplate(t0 p0)
        : data(p0) { }

      template <typename t0, typename t1>
      objectTemplate(t0 p0, t1 p1)
        : data(p0, p1) { }

      template <typename t0, typename t1, typename t2>
      objectTemplate(t0 p0, t1 p1, t2 p2)
        : data(p0, p1, p2) { }

      template <typename t0, typename t1, typename t2, typename t3>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3)
        : data(p0, p1, p2, p3) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4)
        : data(p0, p1, p2, p3, p4) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4,
                typename t5>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4, t5 p5)
        : data(p0, p1, p2, p3, p4, p5) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4,
                typename t5, typename t6>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4, t5 p5, t6 p6)
        : data(p0, p1, p2, p3, p4, p5, p6) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4,
                typename t5, typename t6, typename t7>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4, t5 p5, t6 p6, t7 p7)
        : data(p0, p1, p2, p3, p4, p5, p6, p7) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4,
                typename t5, typename t6, typename t7, typename t8>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4, t5 p5, t6 p6, t7 p7, t8 p8)
        : data(p0, p1, p2, p3, p4, p5, p6, p7, p8) { }

      template <typename t0, typename t1, typename t2, typename t3, typename t4,
                typename t5, typename t6, typename t7, typename t8, typename t9>
      objectTemplate(t0 p0, t1 p1, t2 p2, t3 p3, t4 p4, t5 p5, t6 p6, t7 p7, t8 p8, t9 p9)
        : data(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) { }

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

      static reference getRef(object* o)
      {
        return dynamic_cast<objectTemplate<data_type>&>(*o).data;
      }
  };
}

#endif // TNT_OBJECTTEMPLATE_H

