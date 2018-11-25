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
#include <cxxtools/serializationinfo.h>
#include <tnt/query_params.h>

namespace
{
  struct Sub
  {
    long i;
    std::vector<std::string> a;
  };

  void operator >>= (const cxxtools::SerializationInfo& si, Sub& sub)
  {
    si.getMember("i") >>= sub.i;
    si.getMember("a") >>= sub.a;
  }

}

class QParamTest : public cxxtools::unit::TestSuite
{
  public:
    QParamTest()
      : cxxtools::unit::TestSuite("qparam")
    {
      registerMethod("qparam", *this, &QParamTest::testQParam);
      registerMethod("intarg", *this, &QParamTest::testIntarg);
      registerMethod("defaultValue", *this, &QParamTest::testDefaultValue);
      registerMethod("addValue", *this, &QParamTest::testAddValue);
      registerMethod("object", *this, &QParamTest::testObject);
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

    void testObject()
    {
      // generated with getQuery.js
      tnt::QueryParams q("simple=Hi&numbers%5B%5D=1&numbers%5B%5D=4&numbers%5B%5D=5&sub%5Bi%5D=42&sub%5Ba%5D%5B%5D=Hi&sub%5Ba%5D%5B%5D=there");

      std::string simple = q.get<std::string>("simple");
      std::vector<int> numbers = q.getvector<int>("numbers");
      Sub sub = q.get<Sub>("sub");

      CXXTOOLS_UNIT_ASSERT_EQUALS(simple, "Hi");
      CXXTOOLS_UNIT_ASSERT_EQUALS(numbers.size(), 3);
      CXXTOOLS_UNIT_ASSERT_EQUALS(numbers[0], 1);
      CXXTOOLS_UNIT_ASSERT_EQUALS(numbers[1], 4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(numbers[2], 5);
      CXXTOOLS_UNIT_ASSERT_EQUALS(sub.i, 42);
      CXXTOOLS_UNIT_ASSERT_EQUALS(sub.a.size(), 2);
      CXXTOOLS_UNIT_ASSERT_EQUALS(sub.a[0], "Hi");
      CXXTOOLS_UNIT_ASSERT_EQUALS(sub.a[1], "there");
    }
};

cxxtools::unit::RegisterTest<QParamTest> register_QParamTest;

