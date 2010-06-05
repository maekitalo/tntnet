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

#include <seweb/veranstaltung.h>
#include <cxxtools/serializationinfo.h>

namespace seweb
{
  namespace
  {
    void operator>>= (const cxxtools::SerializationInfo& si, cxxtools::Date& d)
    {
      int year;
      unsigned month;
      unsigned day;
      si.getMember("year") >>= year;
      si.getMember("month") >>= month;
      si.getMember("day") >>= day;
      d.set(year, month, day);
    }

    void operator<<= (cxxtools::SerializationInfo& si, const cxxtools::Date& d)
    {
      si.setTypeName("Date");

      si.addMember("year") <<= d.year();
      si.addMember("month") <<= d.month();
      si.addMember("day") <<= d.day();
    }
  }

  void operator>>= (const cxxtools::SerializationInfo& si, Veranstaltung& v)
  {
    si.getMember("id") >>= v.id;
    si.getMember("name") >>= v.name;
    si.getMember("datum") >>= v.datum;
    si.getMember("ort") >>= v.ort;
    si.getMember("logo") >>= v.logo;
  }

  void operator<<= (cxxtools::SerializationInfo& si, const Veranstaltung& v)
  {
    si.setTypeName("Veranstaltung");

    si.addMember("id") <<= v.id;
    si.addMember("name") <<= v.name;
    si.addMember("datum") <<= v.datum;
    si.addMember("ort") <<= v.ort;
    si.addMember("logo") <<= v.logo;
  }

}
