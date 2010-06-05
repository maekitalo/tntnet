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

#ifndef SEWEB_VERANSTALTUNG_H
#define SEWEB_VERANSTALTUNG_H

#include <string>
#include <cxxtools/date.h>

namespace cxxtools
{
  class SerializationInfo;
}

namespace seweb
{
  struct Veranstaltung
  {
      unsigned id;
      std::string name;
      cxxtools::Date datum;
      std::string ort;
      std::string logo;
  };

  void operator>>= (const cxxtools::SerializationInfo& si, Veranstaltung& v);

  void operator<<= (cxxtools::SerializationInfo& si, const Veranstaltung& v);

}

#endif // SEWEB_VERANSTALTUNG_H
