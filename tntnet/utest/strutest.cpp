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
#include <tnt/stringlessignorecase.h>

class StrTest : public cxxtools::unit::TestSuite
{
    public:
        StrTest()
        : cxxtools::unit::TestSuite("cxxtools-str-Test")
        {
            registerMethod("testCompare_std_string", *this, &StrTest::testCompare_std_string);
            registerMethod("testCompare_charp", *this, &StrTest::testCompare_charp);
        }

        template <typename stringType>
        void testCompareString()
        {
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("a", "B") < 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("A", "b") < 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("B", "a") > 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("b", "A") > 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("a", "aa") < 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("A", "AA") < 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("aa", "a") > 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("AA", "A") > 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("aa", "AA") == 0);
            CXXTOOLS_UNIT_ASSERT(tnt::StringCompareIgnoreCase<stringType>("AA", "aa") == 0);
        }

        void testCompare_std_string()
        {
            testCompareString<std::string>();
        }

        void testCompare_charp()
        {
            testCompareString<const char*>();
        }
};

cxxtools::unit::RegisterTest<StrTest> register_StrTest;
