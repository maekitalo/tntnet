/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <seweb/service/veranstaltung.h>
#include <seweb/impl/veranstaltung.h>
#include <cxxtools/log.h>

log_define("seweb.service.veranstaltung")

namespace seweb
{
namespace service
{

std::vector<seweb::Veranstaltung> Veranstaltung::getVeranstaltungen()
{
  log_trace("getVeranstaltungen()");

  impl::Veranstaltung impl;
  return impl.getVeranstaltungen();
}

seweb::Veranstaltung Veranstaltung::getVeranstaltung(unsigned id)
{
  log_trace("getVeranstaltung(" << id << ')');
  impl::Veranstaltung impl;
  return impl.getVeranstaltung(id);
}

std::vector<seweb::Wettkampf> Veranstaltung::getWettkaempfe(unsigned vid)
{
  log_trace("getWettkaempfe()");
  impl::Veranstaltung impl;
  return impl.getWettkaempfe(vid);
}

seweb::Wettkampf Veranstaltung::getWettkampf(unsigned vid, unsigned wid)
{
  log_trace("getWettkampf(" << vid << ", " << wid << ')');
  impl::Veranstaltung impl;
  return impl.getWettkampf(vid, wid);
}

}
}
