/* tnt/syserror.h
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef TNT_SYSERROR_H
#define TNT_SYSERROR_H

#include <stdexcept>

namespace tnt
{
  class SysError : public std::runtime_error
  {
      static std::string getErrnoString(const char* fn);

    public:
      SysError(const char* fn)
        : std::runtime_error(getErrnoString(fn))
        { }
  };
}

#endif // TNT_SYSERROR_H

