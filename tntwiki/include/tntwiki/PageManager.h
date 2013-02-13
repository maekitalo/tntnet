/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef TNTWIKI_PAGEMANAGER_H
#define TNTWIKI_PAGEMANAGER_H

#include <string>
#include <tntdb/connection.h>
#include <tntdb/statement.h>
#include <cxxtools/datetime.h>
#include <tntwiki/User.h>

namespace tntwiki
{
  class PageManager
  {
    public:
      explicit PageManager(tntdb::Connection conn)
        : _conn(conn)
      { }

      std::string getData(const std::string& title);
      std::string getData(const std::string& title, const cxxtools::DateTime& ts);
      std::string getData(const std::string& title, unsigned version);

      void getTitles(std::vector<std::string>& ret);
      void getTitles(std::vector<std::string>& ret, const cxxtools::DateTime& ts);
      void getTitles(std::vector<std::string>& ret, unsigned version);

      bool hasPage(const std::string& title);
      bool hasPage(const std::string& title, const cxxtools::DateTime& ts);
      bool hasPage(const std::string& title, unsigned version);

      // stores new page; returns new version number
      unsigned update(unsigned userId, const std::string& title, const std::string& data);
      unsigned update(const User& user, const std::string& title, const std::string& data)
        { return update(user.id(), title, data); }

      unsigned del(unsigned userId, const std::string& title);
      unsigned del(const User& user, const std::string& title)
        { return del(user.id(), title); }

    private:
      tntdb::Connection _conn;
      tntdb::Statement _selId;
      tntdb::Statement _selData;
      tntdb::Statement _selDataByTs;
      tntdb::Statement _selDataByVersion;
      tntdb::Statement _selTitles;
      tntdb::Statement _selTitlesByTs;
      tntdb::Statement _selTitlesByVersion;
      tntdb::Statement _selHasPage;
      tntdb::Statement _selHasPageByTs;
      tntdb::Statement _selHasPageByVersion;
      tntdb::Statement _insPage;
      tntdb::Statement _delPage;
  };

}

#endif // TNTWIKI_PAGEMANAGER_H
