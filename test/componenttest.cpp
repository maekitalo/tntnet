/*
 * Copyright (C) 2015 Tommi Maekitalo
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
#include <tnt/cmd.h>
#include <tnt/httpheader.h>
#include <sstream>

class ComponentTest : public cxxtools::unit::TestSuite
{
  public:
    ComponentTest()
    : cxxtools::unit::TestSuite("component")
    {
      registerMethod("testOutput", *this, &ComponentTest::testOutput);
      registerMethod("testArg", *this, &ComponentTest::testArg);
      registerMethod("testScope", *this, &ComponentTest::testScope);
    }

    void testOutput()
    {
      std::ostringstream s;
      tnt::Cmd cmd(s);

      cmd.call(tnt::Compident("output"));

      std::string content = s.str();

      CXXTOOLS_UNIT_ASSERT_EQUALS(content, "f&amp;f&\n&lt;one&gt;\n<one>\n&lt;html&gt;<html>&lt;html&gt;<html>\n&lt;html&gt;<html>");
    }

    void testArg()
    {
      tnt::QueryParams getParams;
      tnt::QueryParams postParams;

      getParams.set("a", "A");
      getParams.set("n", "17");
      getParams.set("ga", "GA");
      getParams.set("gn", "23");

      postParams.set("a", "A");
      postParams.set("n", "17");
      postParams.set("pa", "PA");
      postParams.set("pn", "123");

      {
        std::ostringstream s;
        tnt::Cmd cmd(s);
        cmd.call(tnt::Compident("arg"));
        std::string content = s.str();
        CXXTOOLS_UNIT_ASSERT_EQUALS(content, "a=Hello n=42\nga=Hello gn=43\npa=Hello pn=44\n");
      }

      {
        std::ostringstream s;
        tnt::Cmd cmd(s);
        cmd.call(tnt::Compident("arg"), getParams);
        std::string content = s.str();
        CXXTOOLS_UNIT_ASSERT_EQUALS(content, "a=A n=17\nga=GA gn=23\npa=Hello pn=44\n");
      }

      {
        std::ostringstream s;
        tnt::Cmd cmd(s);
        cmd.request().setMethod("POST");
        cmd.request().setHeader(tnt::httpheader::contentType, "application/x-www-form-urlencoded");
        cmd.request().setBody(postParams.getUrl());
        cmd.call(tnt::Compident("arg"));
        std::string content = s.str();
        CXXTOOLS_UNIT_ASSERT_EQUALS(content, "a=A n=17\nga=Hello gn=43\npa=PA pn=123\n");
      }

    }

    void testScope()
    {
      std::ostringstream s;
      tnt::Cmd cmd(s);

      cmd.call(tnt::Compident("scope"));
      cmd.call(tnt::Compident("scope"));

      std::string content = s.str();

      CXXTOOLS_UNIT_ASSERT_EQUALS(content, "session=1 securesession=1 application=1 request=1\nsession=2 securesession=1 application=2 request=1\n");
    }

};

cxxtools::unit::RegisterTest<ComponentTest> register_ComponentTest;

