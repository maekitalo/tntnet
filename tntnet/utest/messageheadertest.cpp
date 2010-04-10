/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <tnt/messageheader.h>
#include <tnt/messageheaderparser.h>
#include <tnt/httperror.h>
#include <sstream>

class MessageheaderTest : public cxxtools::unit::TestSuite
{
    public:
        MessageheaderTest()
        : cxxtools::unit::TestSuite("cxxtools-messageheader-Test")
        {
            registerMethod("testMessageheader", *this, &MessageheaderTest::testMessageheader);
            registerMethod("testMessageheaderRemove", *this, &MessageheaderTest::testMessageheaderRemove);
            registerMethod("testMessageheaderParser", *this, &MessageheaderTest::testMessageheaderParser);
            registerMethod("testMessageheaderParserSize", *this, &MessageheaderTest::testMessageheaderParserSize);
        }

        void testMessageheader()
        {
            tnt::Messageheader mh;
            mh.setHeader("blah:", "tntnet", false);
            mh.setHeader("foo:", "bar", false);
            mh.setHeader("xyz:", "abc", false);
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("BLAH:", "TNTNET"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("FOO:", "BAR"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("XYZ:", "ABC"));
        }

        void testMessageheaderRemove()
        {
            tnt::Messageheader mh;
            mh.setHeader("blah:", "tntnet", false);
            mh.setHeader("foo:", "bar", false);
            mh.setHeader("xyz:", "abc", false);
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("BLAH:", "TNTNET"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("FOO:", "BAR"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("XYZ:", "ABC"));

            mh.removeHeader("foo:");
            CXXTOOLS_UNIT_ASSERT(!mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));

            mh.removeHeader("xyz:");
            CXXTOOLS_UNIT_ASSERT(!mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(!mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));

            mh.setHeader("xyz:", "abc", false);
            CXXTOOLS_UNIT_ASSERT(!mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));
        }

        void testMessageheaderParser()
        {
            tnt::Messageheader mh;
            tnt::Messageheader::Parser parser(mh);
            std::istringstream in("blah:tntnet\r\nfoo : bar\nXYZ:Abc\r\n\r\n");
            parser.parse(in);
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("foo:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("xyz:"));
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("BLAH:", "TNTNET"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("FOO:", "BAR"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("XYZ:", "ABC"));
        }

        void testMessageheaderParserSize()
        {
            tnt::Messageheader mh;
            tnt::Messageheader::Parser parser(mh);
            for (unsigned c = 0; c < tnt::Messageheader::MAXHEADERSIZE - 1; ++c)
                parser.parse('A');
            CXXTOOLS_UNIT_ASSERT_THROW(parser.parse('B'), tnt::HttpError);
        }

};

cxxtools::unit::RegisterTest<MessageheaderTest> register_MessageheaderTest;
