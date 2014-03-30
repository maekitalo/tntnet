/*
 * Copyright (C) 2013 Tommi Maekitalo
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
#include <tnt/query_params.h>

class QParamTest : public cxxtools::unit::TestSuite
{
    public:
        QParamTest()
        : cxxtools::unit::TestSuite("qparam")
        {
            registerMethod("qparam", *this, &QParamTest::testQParam);
            registerMethod("intarg", *this, &QParamTest::testIntarg);
            registerMethod("locale", *this, &QParamTest::testLocale);
            registerMethod("defaultValue", *this, &QParamTest::testDefaultValue);
            registerMethod("addValue", *this, &QParamTest::testAddValue);
            registerMethod("multipleValues", *this, &QParamTest::testMultipleValues);
        }

        void testQParam()
        {
          tnt::QueryParams q("aa=5&bb=7");
          std::string aa = q.arg<std::string>("aa");
          std::string bb = q.arg<std::string>("bb");
          CXXTOOLS_UNIT_ASSERT_EQUALS(aa, "5");
          CXXTOOLS_UNIT_ASSERT_EQUALS(bb, "7");
        }

        void testIntarg()
        {
          tnt::QueryParams q("aa=5&bb=7");
          int aa = q.arg<int>("aa");
          int bb = q.arg<int>("bb");
          CXXTOOLS_UNIT_ASSERT_EQUALS(aa, 5);
          CXXTOOLS_UNIT_ASSERT_EQUALS(bb, 7);
        }

        void testLocale()
        {
          tnt::QueryParams q("aa=5,5&bb=7,25");
          q.locale(std::locale("de_DE"));
          double aa = q.arg<double>("aa");
          double bb = q.arg<double>("bb");
          CXXTOOLS_UNIT_ASSERT_EQUALS(aa, 5.5);
          CXXTOOLS_UNIT_ASSERT_EQUALS(bb, 7.25);
        }

        void testDefaultValue()
        {
          tnt::QueryParams q;
          std::string aa = q.arg<std::string>("aa", "Hi");
          std::string bb = q.arg<std::string>("bb", "there");
          std::string cc = q.arg<std::string>("cc", "Hi there");
          double dd = q.arg<double>("dd", 42);
          CXXTOOLS_UNIT_ASSERT_EQUALS(aa, "Hi");
          CXXTOOLS_UNIT_ASSERT_EQUALS(bb, "there");
          CXXTOOLS_UNIT_ASSERT_EQUALS(cc, "Hi there");
          CXXTOOLS_UNIT_ASSERT_EQUALS(dd, 42);
        }

        void testAddValue()
        {
          tnt::QueryParams q;
          q.add("a", 17);
          q.add("b", "Hi there");
          q.add("c", true);
          q.add("d", false);
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.arg<unsigned>("a"), 17);
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.arg<std::string>("b"), "Hi there");
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.arg<bool>("c"), true);
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.arg<bool>("d"), false);
        }

        void testMultipleValues()
        {
          tnt::QueryParams q("a=17&a=4&b=Hi&a=28");
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("a"), 3);
          CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("b"), 1);

          std::vector<int> a = q.args<int>("a");

          CXXTOOLS_UNIT_ASSERT_EQUALS(a.size(), 3);
          CXXTOOLS_UNIT_ASSERT_EQUALS(a[0], 17);
          CXXTOOLS_UNIT_ASSERT_EQUALS(a[1], 4);
          CXXTOOLS_UNIT_ASSERT_EQUALS(a[2], 28);
        }

};

cxxtools::unit::RegisterTest<QParamTest> register_QParamTest;
