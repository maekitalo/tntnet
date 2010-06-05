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

#ifndef SEWEB_SERVICE_VERANSTALTUNG_H
#define SEWEB_SERVICE_VERANSTALTUNG_H

#include <cxxtools/xmlrpc/service.h>
#include <seweb/veranstaltung.h>
#include <seweb/wettkampf.h>

namespace seweb
{
namespace service
{

class Veranstaltung : public cxxtools::xmlrpc::Service
{
  public:
    Veranstaltung()
    {
      registerMethod("getVeranstaltungen", *this, &Veranstaltung::getVeranstaltungen);
      registerMethod("getVeranstaltung", *this, &Veranstaltung::getVeranstaltung);
      registerMethod("getWettkaempfe", *this, &Veranstaltung::getWettkaempfe);
      registerMethod("getWettkampf", *this, &Veranstaltung::getWettkampf);
    }

    std::vector<seweb::Veranstaltung> getVeranstaltungen();
    seweb::Veranstaltung getVeranstaltung(unsigned id);

    std::vector<seweb::Wettkampf> getWettkaempfe(unsigned vid);
    seweb::Wettkampf getWettkampf(unsigned vid, unsigned wid);
};

}
}

#endif // SEWEB_SERVICE_VERANSTALTUNG_H
