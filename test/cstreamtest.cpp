/*
 * Copyright (C) 2014 Tommi Maekitalo
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
#include <tnt/cstream.h>

class cstreamTest : public cxxtools::unit::TestSuite
{
    public:
      cstreamTest()
        : cxxtools::unit::TestSuite("cstream")
      {
        registerMethod("testWrite", *this, &cstreamTest::testWrite);
        registerMethod("testRollback", *this, &cstreamTest::testRollback);
      }

      void doTestWrite(const char* testString, unsigned size, unsigned chunksize, unsigned chunkcount)
      {
        tnt::ocstream s(chunksize);
        s << testString;
        CXXTOOLS_UNIT_ASSERT_EQUALS(testString, str(s));
        CXXTOOLS_UNIT_ASSERT_EQUALS(s.chunkcount(), chunkcount);
      }

      void testWrite()
      {
        const char testString1[] = "Hi there";
        const char testString2[] = "ABCDEF";
        const char testString3[] = "1234567890";
        doTestWrite(testString1, sizeof(testString1), 4, 2);
        doTestWrite(testString2, sizeof(testString2), 5, 2);
        doTestWrite(testString3, sizeof(testString3), 2, 5);
      }

      std::string str(const tnt::ocstream& s)
      {
        std::string result;
        for (tnt::ocstream::size_type n = 0; n < s.chunkcount(); ++n)
          result.append(s.chunk(n), s.chunksize(n));
        return result;
      }

      void testRollback()
      {
        const char* testString = "Hi there";

        tnt::ocstream s(5);
        s << testString;

        s.rollback(3);

        CXXTOOLS_UNIT_ASSERT_EQUALS("Hi ", str(s));
        CXXTOOLS_UNIT_ASSERT_EQUALS(s.chunkcount(), 1);

        s << "there";

        CXXTOOLS_UNIT_ASSERT_EQUALS("Hi there", str(s));
        CXXTOOLS_UNIT_ASSERT_EQUALS(s.chunkcount(), 2);

        s.rollback(0);

        CXXTOOLS_UNIT_ASSERT_EQUALS("", str(s));
      }

};

cxxtools::unit::RegisterTest<cstreamTest> register_cstreamTest;

