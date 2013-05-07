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
#include <tnt/stringlessignorecase.h>

class StrTest : public cxxtools::unit::TestSuite
{
    public:
        StrTest()
        : cxxtools::unit::TestSuite("str-Test")
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
