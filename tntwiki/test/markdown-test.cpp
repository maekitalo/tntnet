/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 */

#include <tntwiki/MarkdownParser.h>
#include <tntwiki/MarkdownHtml.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <sstream>

class MarkdownTest : public cxxtools::unit::TestSuite
{
  public:
    MarkdownTest()
      : cxxtools::unit::TestSuite("markdown")
    {
      registerMethod("plain", *this, &MarkdownTest::testPlain);
      registerMethod("spanelement", *this, &MarkdownTest::testSpanElement);
      registerMethod("para", *this, &MarkdownTest::testPara);
      registerMethod("header-ul", *this, &MarkdownTest::testHeaderUl);
      registerMethod("header-atx", *this, &MarkdownTest::testHeaderAtx);
      registerMethod("blockquote", *this, &MarkdownTest::testBlockquote);
      registerMethod("code", *this, &MarkdownTest::testCode);
      registerMethod("list", *this, &MarkdownTest::testList);
      registerMethod("list-at-end", *this, &MarkdownTest::testListAtEnd);
      registerMethod("nested-list", *this, &MarkdownTest::testNestedList);
      registerMethod("orderedlist", *this, &MarkdownTest::testOrderedList);
      registerMethod("orderedlist-at-end", *this, &MarkdownTest::testOrderedListAtEnd);
      registerMethod("link", *this, &MarkdownTest::testLink);
      registerMethod("linkWithData", *this, &MarkdownTest::testLinkWithData);
      registerMethod("linkTitle", *this, &MarkdownTest::testLinkTitle);
    }

    void testPlain()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse("Hello world!");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "<p>Hello world!</p>\n");
    }

    void testSpanElement()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse("__Hello__ *world!*");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "<p><strong>Hello</strong> <em>world!</em></p>\n");
    }

    void testPara()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        "Hello world!\n"
        "\n"
        "foobar\n"
        "\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<p>Hello world!</p>\n"
        "<p>foobar</p>\n");
    }

    void testHeaderUl()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        "Hello world!\n"
        "=========\n"
        "foobar\n"
        "\n"
        "Blub\n"
        "----\n");

      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<h1>Hello world!</h1>\n"
        "<p>foobar</p>\n"
        "<h2>Blub</h2>\n");
    }

    void testHeaderAtx()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse("##### Hello world!\n\nfoobar\n\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), "<h5>Hello world!</h5>\n<p>\nfoobar</p>\n");
    }

    void testBlockquote()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        "Hello\n"
        "\n"
        "> this is blockquote\n\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<p>Hello</p>\n"
        "<blockquote>\n"
        "<p>this is blockquote</p>\n"
        "</blockquote>\n");
    }

    void testCode()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        "    this is code\n"
        "    more code\n\n"
        "    and even more\n"
        "And this is no code any more");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<code>this is code\n"
        "more code\n"
        "\n"
        "and even more\n"
        "</code>\n"
        "<p>And this is no code any more</p>\n");
    }

    void testList()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        " * Red\n"
        " * Green\n"
        " * Blue\n"
        " blub\n"
        "\n"
        "other stuff\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<ul>\n"
        "<li>Red</li>\n"
        "<li>Green</li>\n"
        "<li>Blue\n"
        "blub</li>\n"
        "</ul>\n"
        "<p>other stuff</p>\n");
    }

    void testListAtEnd()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        " * Red\n"
        " * Green\n"
        " * Blue\n"
        " blub\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<ul>\n"
        "<li>Red</li>\n"
        "<li>Green</li>\n"
        "<li>Blue\n"
        "blub</li>\n"
        "</ul>\n");
    }

    void testNestedList()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        " * Red\n"
        " * Green\n"
        "  + one\n"
        "  + two\n"
        "  + three\n"
        " * Blue\n"
        " blub\n"
        "\n"
        "other stuff\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<ul>\n"
        "<li>Red</li>\n"
        "<li>Green<ul>\n"
        "<li>one</li>\n"
        "<li>two</li>\n"
        "<li>three</li>\n"
        "</ul>\n"
        "</li>\n"
        "<li>Blue\n"
        "blub</li>\n"
        "</ul>\n"
        "<p>other stuff</p>\n");
    }

    void testOrderedList()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        " 1. one\n"
        " 2. two\n"
        " 3. three\n"
        " blub\n"
        "\n"
        "other stuff\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<ol>\n"
        "<li>one</li>\n"
        "<li>two</li>\n"
        "<li>three\n"
        "blub</li>\n"
        "</ol>\n"
        "<p>other stuff</p>\n");
    }

    void testOrderedListAtEnd()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
        " 1. one\n"
        " 2. two\n"
        " 3. three\n"
        " blub\n");
      CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(),
        "<ol>\n"
        "<li>one</li>\n"
        "<li>two</li>\n"
        "<li>three\n"
        "blub</li>\n"
        "</ol>\n");
    }

    void testLink()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
          "[tntnet] (http://www.tntnet.org/)");
      CXXTOOLS_UNIT_ASSERT_EQUALS(
          out.str(),
          "<p><a href=\"http://www.tntnet.org/\">tntnet</a></p>\n");
    }

    void testLinkWithData()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
          "Hi [tntnet] (http://www.tntnet.org/) there");
      CXXTOOLS_UNIT_ASSERT_EQUALS(
          out.str(),
          "<p>Hi <a href=\"http://www.tntnet.org/\">tntnet</a> there</p>\n");
    }

    void testLinkTitle()
    {
      std::ostringstream out;
      tntwiki::markdown::Html html(out);
      tntwiki::markdown::Parser parser(html);
      parser.parse(
          "[tntnet] (http://www.tntnet.org/ 'Tntnet homepage')");
      CXXTOOLS_UNIT_ASSERT_EQUALS(
          out.str(),
          "<p><a href=\"http://www.tntnet.org/\" title=\"Tntnet homepage\">tntnet</a></p>\n");
    }

};

cxxtools::unit::RegisterTest<MarkdownTest> register_MarkdownTest;
