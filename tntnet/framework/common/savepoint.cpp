/* savepoint.cpp
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
