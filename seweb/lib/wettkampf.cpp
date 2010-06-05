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

#include <seweb/wettkampf.h>
#include <cxxtools/serializationinfo.h>

namespace seweb
{
  namespace
  {
    void operator>>= (const cxxtools::SerializationInfo& si, cxxtools::Time& t)
    {
      unsigned hour;
      unsigned minute;
      unsigned second;
      unsigned msec;
      si.getMember("hour") >>= hour;
      si.getMember("minute") >>= minute;
      si.getMember("second") >>= second;
      si.getMember("msec") >>= msec;
      t.set(hour, minute, second, msec);
    }

    void operator<<= (cxxtools::SerializationInfo& si, const cxxtools::Time& t)
    {
      si.addMember("hour") <<= t.hour();
      si.addMember("minute") <<= t.minute();
      si.addMember("second") <<= t.second();
      si.addMember("msec") <<= t.msec();
    }
  }

  void operator>>= (const cxxtools::SerializationInfo& si, Wettkampf& w)
  {
    si.getMember("id") >>= w.id;
    si.getMember("name") >>= w.name;
    si.getMember("art") >>= w.art;
    si.getMember("staVon") >>= w.staVon;
    si.getMember("staBis") >>= w.staBis;
    si.getMember("startzeit") >>= w.startzeit;
  }

  void operator<<= (cxxtools::SerializationInfo& si, const Wettkampf& w)
  {
    si.addMember("id") <<= w.id;
    si.addMember("name") <<= w.name;
    si.addMember("art") <<= w.art;
    si.addMember("staVon") <<= w.staVon;
    si.addMember("staBis") <<= w.staBis;
    si.addMember("startzeit") <<= w.startzeit;
  }

}
