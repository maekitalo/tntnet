/* tnt/savepoint.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#ifndef TNT_SAVEPOINT_H
#define TNT_SAVEPOINT_H

#include <tnt/httpreply.h>

namespace tnt
{
  class Savepoint
  {
      bool active;
      HttpReply& reply;
      std::string::size_type pos;

    public:
      Savepoint(HttpReply& reply_, bool active_ = true)
        : active(false),
          reply(reply_)
      {
        if (active_)
          save();
      }
      ~Savepoint()
      {
        if (active)
          rollback();
      }

      void save();
      void commit();
      void rollback();
  };
}

#endif // TNT_SAVEPOINT_H
