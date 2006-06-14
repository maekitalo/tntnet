/* savepoint.cpp
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

#include <tnt/savepoint.h>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.savepoint")

  void Savepoint::save()
  {
    pos = reply.outstream.str().size();
    active = true;

    log_debug("set Savepoint " << pos);
  }

  void Savepoint::commit()
  {
    log_debug("commit Savepoint " << pos);
    active = false;
  }

  void Savepoint::rollback()
  {
    if (active)
    {
      log_info("rollback to Savepoint " << pos);

      reply.outstream.str( reply.outstream.str().substr(0, pos) );
      reply.outstream.seekp(pos);
      active = false;
    }
    else
      log_error("not rolling back not active Savepoint");
  }

}
