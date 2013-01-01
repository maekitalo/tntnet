/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tntwiki/Renderer.h>
#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

const std::string dburl = "postgresql:dbname=tntwiki";

class RendererTest : public cxxtools::unit::TestSuite
{
  public:
    RendererTest()
      : cxxtools::unit::TestSuite("renderer")
    {
      registerMethod("header1", *this, &RendererTest::testHeader1);
      registerMethod("header2", *this, &RendererTest::testHeader2);
      registerMethod("header1_2", *this, &RendererTest::testHeader1_2);
      registerMethod("bold", *this, &RendererTest::testBold);
      registerMethod("italic", *this, &RendererTest::testItalic);
      registerMethod("combined", *this, &RendererTest::testCombined);
    }

    void testHeader1()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "header1 == Hi == end");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "header1 <h1>Hi</h1> end");
    }

    void testHeader2()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "header1 === Hi === end");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "header1 <h2>Hi</h2> end");
    }

    void testHeader1_2()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "header1 == blub == === Hi === end");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "header1 <h1>blub</h1> <h2>Hi</h2> end");
    }

    void testBold()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "test '''bold text''' test");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "test <b>bold text</b> test");
    }

    void testItalic()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "test ''italic text'' test");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "test <i>italic text</i> test");
    }

    void testCombined()
    {
      tntdb::Connection conn = tntdb::connectCached(dburl);
      tntwiki::Renderer renderer(conn);

      std::ostringstream result;
      renderer.putHtml(result, "== header1 ==\n=== header2 ===\n'''bold''' and ''italic'' text");
      CXXTOOLS_UNIT_ASSERT_EQUALS(result.str(), "<h1>header1</h1>\n<h2>header2</h2>\n<b>bold</b> and <i>italic</i> text");
    }
};

cxxtools::unit::RegisterTest<RendererTest> register_RendererTest;
