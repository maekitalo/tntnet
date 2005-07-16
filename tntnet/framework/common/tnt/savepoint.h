/* tnt/savepoint.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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
