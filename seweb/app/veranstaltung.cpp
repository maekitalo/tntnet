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
#include <seweb/configuration.h>
#include <seweb/veranstaltung.h>
#include <seweb/wettkampf.h>
#include <tntdb/connect.h>
#include <tntdb/statement.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <cxxtools/log.h>

log_define("seweb.service.veranstaltung")

namespace seweb
{
namespace service
{

namespace
{
  bool operator>> (const tntdb::Value& value, cxxtools::Date& date)
  {
    tntdb::Date d;
    bool isNotNull = (value >> d);
    if (isNotNull)
      date.set(d.getYear(), d.getMonth(), d.getDay());
    return isNotNull;
  }

  bool operator>> (const tntdb::Value& value, cxxtools::Time& time)
  {
    tntdb::Time t;
    bool isNotNull = (value >> t);
    if (isNotNull)
      time.set(t.getHour(), t.getMinute(), t.getSecond(), t.getMillis());
    return isNotNull;
  }

}

std::vector<seweb::Veranstaltung> Veranstaltung::getVeranstaltungen()
{
  log_trace("getVeranstaltungen()");

  seweb::Configuration conf = seweb::Configuration::get();
  tntdb::Connection conn = tntdb::connectCached(conf.dburl());

  tntdb::Statement stmt = conn.prepareCached(
    "select van_vid, van_name, van_datum, van_ort, van_logo"
    "  from veranstaltung"
    " order by van_datum", "van");

  std::vector<seweb::Veranstaltung> ret;
  for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur)
  {
    tntdb::Row row = *cur;

    seweb::Veranstaltung v;

    row[0] >> v.id;
    row[1] >> v.name;
    row[2] >> v.datum;
    row[3] >> v.ort;
    row[4] >> v.logo;

    ret.push_back(v);
  }

  return ret;
}

seweb::Veranstaltung Veranstaltung::getVeranstaltung(unsigned id)
{
  log_trace("getVeranstaltung(" << id << ')');

  seweb::Configuration conf = seweb::Configuration::get();
  tntdb::Connection conn = tntdb::connectCached(conf.dburl());

  tntdb::Statement stmt = conn.prepareCached(
    "select van_name, van_datum, van_ort, van_logo"
    "  from veranstaltung"
    " where van_vid = :id", "van_id");

  seweb::Veranstaltung ret;
  tntdb::Row row = stmt.set("id", id).selectRow();
  ret.id = id;
  row[0] >> ret.name;
  row[1] >> ret.datum;
  row[2] >> ret.ort;
  row[3] >> ret.logo;

  return ret;
}

std::vector<seweb::Wettkampf> Veranstaltung::getWettkaempfe(unsigned vid)
{
  log_trace("getWettkaempfe()");

  seweb::Configuration conf = seweb::Configuration::get();
  tntdb::Connection conn = tntdb::connectCached(conf.dburl());

  tntdb::Statement stmt = conn.prepareCached(
    "select wet_wid, wet_name, wet_art, wet_sta_von, wet_sta_bis, wet_startzeit"
    "  from wettkampf"
    " where wet_vid = :vid"
    " order by wet_wid", "wet");

  stmt.set("vid", vid);

  std::vector<seweb::Wettkampf> ret;
  for (tntdb::Statement::const_iterator cur = stmt.begin(); cur != stmt.end(); ++cur)
  {
    ret.push_back(seweb::Wettkampf());
    seweb::Wettkampf& v = ret.back();
    tntdb::Row row = *cur;
    row[0] >> v.id;
    row[1] >> v.name;
    row[2] >> v.art;
    row[3] >> v.staVon;
    row[4] >> v.staBis;
    row[5] >> v.startzeit;
  }

  return ret;
}

seweb::Wettkampf Veranstaltung::getWettkampf(unsigned vid, unsigned wid)
{
  log_trace("getWettkampf(" << vid << ", " << wid << ')');

  seweb::Configuration conf = seweb::Configuration::get();
  tntdb::Connection conn = tntdb::connectCached(conf.dburl());

  tntdb::Statement stmt = conn.prepareCached(
    "select wet_name, wet_art, wet_sta_von, wet_sta_bis, wet_startzeit"
    "  from wettkampf"
    " where wet_vid = :vid"
    "   and wet_wid = :wid");

  seweb::Wettkampf ret;
  tntdb::Row row = stmt.set("vid", vid)
                       .set("wid", wid)
                       .selectRow();

  ret.id = wid;
  row[0] >> ret.name;
  row[1] >> ret.art;
  row[2] >> ret.staVon;
  row[3] >> ret.staBis;
  row[4] >> ret.startzeit;

  return ret;
}

}
}
