/*
 * Copyright (C) 2009 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
        : cxxtools::unit::TestSuite("messageheader-Test")
        {
            registerMethod("testMessageheader", *this, &MessageheaderTest::testMessageheader);
            registerMethod("testMessageheaderColon", *this, &MessageheaderTest::testMessageheaderColon);
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

        void testMessageheaderColon()
        {
            tnt::Messageheader mh;
            mh.setHeader("blah", "tntnet", false);
            CXXTOOLS_UNIT_ASSERT(mh.hasHeader("blah:"));
            CXXTOOLS_UNIT_ASSERT(mh.compareHeader("BLAH:", "TNTNET"));
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
