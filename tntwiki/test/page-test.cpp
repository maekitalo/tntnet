/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tntwiki/PageManager.h>
#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

const std::string dburl = "postgresql:dbname=tntwiki";

class PageTest : public cxxtools::unit::TestSuite
{
  public:
    PageTest()
      : cxxtools::unit::TestSuite("page")
    {
      registerMethod("createPage", *this, &PageTest::testCreatePage);
      registerMethod("updatePage", *this, &PageTest::testUpdatePage);
      registerMethod("hasPage", *this, &PageTest::testHasPage);
      registerMethod("delPage", *this, &PageTest::testDelPage);
      registerMethod("getTitles", *this, &PageTest::testGetTitles);
    }

    void testCreatePage()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::PageManager pageManager(conn);

      tntdb::Transaction trans(conn);

      std::string title1 = "Page1";
      std::string content1 = "Content of page 1";
      std::string title2 = "Page2";
      std::string content2 = "Content of page 2";

      pageManager.update(1, title1, content1);
      pageManager.update(1, title2, content2);

      std::string content1_ = pageManager.getData(title1);
      std::string content2_ = pageManager.getData(title2);

      CXXTOOLS_UNIT_ASSERT_EQUALS(content1, content1_);
      CXXTOOLS_UNIT_ASSERT_EQUALS(content2, content2_);
    }

    void testUpdatePage()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::PageManager pageManager(conn);

      tntdb::Transaction trans(conn);

      std::string title1 = "Page1";
      std::string content1 = "Content of page 1";
      pageManager.update(1, title1, content1);

      std::string content1_ = pageManager.getData(title1);

      CXXTOOLS_UNIT_ASSERT_EQUALS(content1, content1_);

      content1 = "New content of page 1";
      pageManager.update(1, title1, content1);

      content1_ = pageManager.getData(title1);

      CXXTOOLS_UNIT_ASSERT_EQUALS(content1, content1_);
    }

    void testHasPage()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::PageManager pageManager(conn);

      tntdb::Transaction trans(conn);

      std::string title1 = "Page1";
      std::string content1 = "Content of page 1";
      std::string title2 = "Page2";
      std::string content2 = "Content of page 2";

      pageManager.update(1, title1, content1);
      pageManager.update(1, title2, content2);

      CXXTOOLS_UNIT_ASSERT(pageManager.hasPage("Page1"));
      CXXTOOLS_UNIT_ASSERT(pageManager.hasPage("Page2"));
      CXXTOOLS_UNIT_ASSERT(!pageManager.hasPage("Page3"));
    }

    void testDelPage()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::PageManager pageManager(conn);

      tntdb::Transaction trans(conn);

      std::string title1 = "Page1";
      std::string content1 = "Content of page 1";
      std::string title2 = "Page2";
      std::string content2 = "Content of page 2";

      pageManager.update(1, title1, content1);
      pageManager.update(1, title2, content2);

      CXXTOOLS_UNIT_ASSERT(pageManager.hasPage("Page1"));
      CXXTOOLS_UNIT_ASSERT(pageManager.hasPage("Page2"));
      CXXTOOLS_UNIT_ASSERT(!pageManager.hasPage("Page3"));

      pageManager.del(1, "Page2");

      CXXTOOLS_UNIT_ASSERT(pageManager.hasPage("Page1"));
      CXXTOOLS_UNIT_ASSERT(!pageManager.hasPage("Page2"));
      CXXTOOLS_UNIT_ASSERT(!pageManager.hasPage("Page3"));
    }

    void testGetTitles()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::PageManager pageManager(conn);

      tntdb::Transaction trans(conn);

      pageManager.update(1, "Page2", "content2");
      pageManager.update(1, "Page3", "content3");
      pageManager.update(1, "Page1", "content1");

      std::vector<std::string> titles;
      pageManager.getTitles(titles);

      CXXTOOLS_UNIT_ASSERT_EQUALS(titles.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(titles[0], "Page1");
      CXXTOOLS_UNIT_ASSERT_EQUALS(titles[1], "Page2");
      CXXTOOLS_UNIT_ASSERT_EQUALS(titles[2], "Page3");

      pageManager.del(1, "Page2");
      titles.clear();
      pageManager.getTitles(titles);

      CXXTOOLS_UNIT_ASSERT_EQUALS(titles.size(), 2);
      CXXTOOLS_UNIT_ASSERT_EQUALS(titles[0], "Page1");
      CXXTOOLS_UNIT_ASSERT_EQUALS(titles[1], "Page3");
    }

};

cxxtools::unit::RegisterTest<PageTest> register_PageTest;
