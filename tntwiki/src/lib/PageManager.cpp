/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tntwiki/PageManager.h>
#include <tntdb/transaction.h>
#include <tntdb/cxxtools/datetime.h>

namespace tntwiki
{

std::string PageManager::getData(const std::string& title)
{
  if (!_selData)
    _selData = _conn.prepareCached(
      "select data"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title)");

  std::string ret;

  _selData.set("title", title)
          .selectValue()
          .get(ret);

  return ret;
}

std::string PageManager::getData(const std::string& title, const cxxtools::DateTime& ts)
{
  if (!_selDataByTs)
    _selDataByTs = _conn.prepareCached(
      "select data"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title"
      "                and ts <= :ts)");

  std::string ret;

  _selDataByTs.set("title", title)
              .set("ts", ts)
              .selectValue()
              .get(ret);

  return ret;
}

std::string PageManager::getData(const std::string& title, unsigned version)
{
  if (!_selDataByVersion)
    _selDataByVersion = _conn.prepareCached(
      "select data"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title"
      "                and id <= :id)");

  std::string ret;

  _selDataByVersion.set("title", title)
                   .set("id", version)
                   .selectValue()
                   .get(ret);

  return ret;
}

void PageManager::getTitles(std::vector<std::string>& ret)
{
  if (!_selTitles)
    _selTitles = _conn.prepareCached(
      "select p1.title"
      "  from pages p1"
      "  join pages p2"
      "    on p1.title = p2.title"
      " where p1.data is not null"
      " group by p1.id, p1.title"
      " having p1.id = max(p2.id)"
      " order by 1");

  for (tntdb::Statement::const_iterator cur = _selTitles.begin();
    cur != _selTitles.end(); ++cur)
  {
    ret.resize(ret.size() + 1);
    (*cur)[0].get(ret.back());
  }
}

void PageManager::getTitles(std::vector<std::string>& ret, const cxxtools::DateTime& ts)
{
  if (!_selTitlesByTs)
    _selTitlesByTs = _conn.prepareCached(
      "select p1.title"
      "  from pages p1"
      "  join pages p2"
      "    on p1.title = p2.title"
      "   and p2.ts <= :ts"
      " where p1.data is not null"
      " group by p1.id, p1.title"
      " having p1.id = max(p2.id)"
      "  order by 1");

  _selTitlesByTs.set("ts", ts);

  for (tntdb::Statement::const_iterator cur = _selTitlesByTs.begin();
    cur != _selTitlesByTs.end(); ++cur)
  {
    ret.resize(ret.size() + 1);
    (*cur)[0].get(ret.back());
  }
}

void PageManager::getTitles(std::vector<std::string>& ret, unsigned version)
{
  if (!_selTitlesByVersion)
    _selTitlesByVersion = _conn.prepareCached(
      "select p1.title"
      "  from pages p1"
      "  join pages p2"
      "    on p1.title = p2.title"
      "   and p2.id <= :id"
      " where p1.data is not null"
      " group by p1.id, p1.title"
      " having p1.id = max(p2.id)"
      "  order by 1");

  _selTitlesByVersion.set("id", version);

  for (tntdb::Statement::const_iterator cur = _selTitlesByVersion.begin();
    cur != _selTitlesByVersion.end(); ++cur)
  {
    ret.resize(ret.size() + 1);
    (*cur)[0].get(ret.back());
  }
}

bool PageManager::hasPage(const std::string& title)
{
  if (!_selHasPage)
    _selHasPage = _conn.prepareCached(
      "select max(1)"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title)"
      "   and data is not null");

  return !_selHasPage.set("title", title)
                     .selectValue()
                     .isNull();
}

bool PageManager::hasPage(const std::string& title, const cxxtools::DateTime& ts)
{
  if (!_selHasPageByTs)
    _selHasPageByTs = _conn.prepareCached(
      "select max(1)"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title)"
      "                and ts <= :ts)"
      "   and data is not null");

  return !_selHasPageByTs.set("title", title)
                         .set("ts", ts)
                         .selectValue()
                         .isNull();
}


bool PageManager::hasPage(const std::string& title, unsigned version)
{
  if (!_selHasPageByVersion)
    _selHasPageByVersion = _conn.prepareCached(
      "select max(1)"
      "  from pages"
      " where title = :title"
      "   and id = (select max(id)"
      "               from pages"
      "              where title = :title)"
      "                and id <= :version)"
      "   and data is not null");

  return !_selHasPageByVersion.set("title", title)
                              .set("id", version)
                              .selectValue()
                              .isNull();
}


unsigned PageManager::update(unsigned userId, const std::string& title, const std::string& data)
{
  if (!_selId)
    _selId = _conn.prepareCached(
      "select max(id) + 1"
      "  from pages");

  if (!_insPage)
    _insPage = _conn.prepareCached(
      "insert into pages(id, title, ts, data, userid)"
      " values (:id, :title, 'now', :data, :userid)");

  tntdb::Transaction trans(_conn);
#ifdef HAVLE_TNTDB_TABLELOCKER
  tntdb::TableLocker tableLocker(_conn, "pages", true);
#else
  _conn.execute("lock table pages in exclusive mode");
#endif

  tntdb::Value id_ = _selId.selectValue();
  unsigned id = 1;
  id_.get(id);  // get returns false, if id is not found and does not modify id

  _insPage.set("id", id)
          .set("title", title)
          .set("data", data)
          .set("userid", userId)
          .execute();

  trans.commit();

  return id;
}

unsigned PageManager::del(unsigned userId, const std::string& title)
{
  if (!_selId)
    _selId = _conn.prepareCached(
      "select max(id) + 1"
      "  from pages");

  if (!_delPage)
    _delPage = _conn.prepareCached(
      "insert into pages(id, title, ts, userid)"
      " values (:id, :title, 'now', :userid)");

  tntdb::Transaction trans(_conn);
  _conn.execute("lock table pages in exclusive mode");

  unsigned id = 1;
  _selId.selectValue()
        .get(id);   // get returns false, if id is not found and does not modify id

  _delPage.set("id", id)
          .set("title", title)
          .set("userid", userId)
          .execute();

  trans.commit();

  return id;
}

}
