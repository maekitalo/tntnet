/* tnt/ecpp/scopetypes.h
 * Copyright (C) 2005 Tommi Maekitalo
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


#ifndef TNT_ECPP_SCOPETYPES_H
#define TNT_ECPP_SCOPETYPES_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <list>

namespace tnt
{
  namespace ecpp
  {
    enum scope_container_type
    {
      application_container,
      thread_container,
      session_container,
      request_container,
      param_container
    };

    enum scope_type
    {
      global_scope,
      page_scope,
      component_scope
    };
  }
}

#endif // TNT_ECPP_SCOPETYPES_H

