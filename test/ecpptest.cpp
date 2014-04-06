/*
 * Copyright (C) 2012 Tommi Maekitalo
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
#include <tnt/ecpp/parsehandler.h>
#include <tnt/ecpp/parser.h>

namespace
{
  class Handler : public tnt::ecpp::ParseHandler
  {
      std::ostringstream _result;

      virtual void start();
      virtual void end();
      virtual void onLine(unsigned lineno, const std::string& file);
      virtual void onHtml(const std::string& html);
      virtual void onExpression(const std::string& expr);
      virtual void onHtmlExpression(const std::string& expr);
      virtual void onCpp(const std::string& code);
      virtual void onPre(const std::string& code);
      virtual void onInit(const std::string& code);
      virtual void onCleanup(const std::string& code);
      virtual void onArg(const std::string& name,
        const std::string& value);
      virtual void onAttr(const std::string& name,
        const std::string& value);
      virtual void onCall(const std::string& comp,
        const comp_args_type& args, const std::string& pass_cgi,
        const paramargs_type& paramargs, const std::string& cppargs);
      virtual void onEndCall(const std::string& comp);
      virtual void onShared(const std::string& code);
      virtual void onScope(tnt::ecpp::scope_container_type container, tnt::ecpp::scope_type scope,
        const std::string& type, const std::string& var, const std::string& init,
        const std::vector<std::string>& includes);
      virtual void startComp(const std::string& name, const cppargs_type& cppargs);
      virtual void startClose();
      virtual void endClose();
      virtual void onComp(const std::string& code);
      virtual void onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr);
      virtual void onConfig(const std::string& code, const std::string& value);
      virtual void tokenSplit(bool start);
      virtual void onInclude(const std::string& file);
      virtual void onIncludeEnd(const std::string& file);
      virtual void startI18n();
      virtual void endI18n();

    public:
      void clear()
      { _result.clear(); _result.str(std::string()); }

      std::string result() const
      { return _result.str(); }
  };

  void Handler::start()
  {
    _result << "start()";
  }

  void Handler::end()
  {
    _result << "end()";
  }

  void Handler::onLine(unsigned lineno, const std::string& file)
  {
  }

  void Handler::onHtml(const std::string& html)
  {
    _result << "onHtml(" << html << ')';
  }

  void Handler::onExpression(const std::string& expr)
  {
    _result << "onExpression(" << expr << ')';
  }

  void Handler::onHtmlExpression(const std::string& expr)
  {
    _result << "onHtmlExpression(" << expr << ')';
  }

  void Handler::onCpp(const std::string& code)
  {
    _result << "onCpp(" << code << ')';
  }

  void Handler::onPre(const std::string& code)
  {
    _result << "onPre(" << code << ')';
  }

  void Handler::onInit(const std::string& code)
  {
    _result << "onInit(" << code << ')';
  }

  void Handler::onCleanup(const std::string& code)
  {
    _result << "onCleanup(" << code << ')';
  }

  void Handler::onArg(const std::string& name, const std::string& value)
  {
    _result << "onArg(" << name << ", " << value << ')';
  }

  void Handler::onAttr(const std::string& name, const std::string& value)
  {
    _result << "onAttr(" << name << ", " << value << ')';
  }

  void Handler::onCall(const std::string& comp, const comp_args_type& args, const std::string& pass_cgi, const paramargs_type& paramargs, const std::string& cppargs)
  {
    _result << "onCall()";
  }

  void Handler::onEndCall(const std::string& comp)
  {
    _result << "onEndCall()";
  }

  void Handler::onShared(const std::string& code)
  {
    _result << "onShared()";
  }

  void Handler::onScope(tnt::ecpp::scope_container_type container, tnt::ecpp::scope_type scope, const std::string& type, const std::string& var, const std::string& init,
          const std::vector<std::string>& includes)
  {
    _result << "onScope(";

    switch (container)
    {
      case tnt::ecpp::application_container: _result << "application"; break;
      case tnt::ecpp::thread_container: _result << "thread"; break;
      case tnt::ecpp::session_container: _result << "session"; break;
      case tnt::ecpp::secure_session_container: _result << "secure_session"; break;
      case tnt::ecpp::request_container: _result << "request"; break;
      case tnt::ecpp::param_container: _result << "param"; break;
    }

    _result << ',';

    switch (scope)
    {
      case tnt::ecpp::default_scope: _result << "default"; break;
      case tnt::ecpp::shared_scope: _result << "shared"; break;
      case tnt::ecpp::page_scope: _result << "page"; break;
      case tnt::ecpp::component_scope: _result << "component"; break;
    }

    _result << ',' << type << ',' << var << ',' << init << ')';
  }

  void Handler::startComp(const std::string& name, const cppargs_type& cppargs)
  {
    _result << "startComp()";
  }

  void Handler::startClose()
  {
    _result << "startClose()";
  }

  void Handler::endClose()
  {
    _result << "endClose()";
  }

  void Handler::onComp(const std::string& code)
  {
    _result << "onComp()";
  }

  void Handler::onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr)
  {
    _result << "onCondExpr()";
  }

  void Handler::onConfig(const std::string& code, const std::string& value)
  {
    _result << "onConfig()";
  }

  void Handler::tokenSplit(bool start)
  {
    _result << "tokenSplit()";
  }

  void Handler::onInclude(const std::string& file)
  {
    _result << "onInclude()";
  }

  void Handler::onIncludeEnd(const std::string& file)
  {
    _result << "onIncludeEnd()";
  }

  void Handler::startI18n()
  {
    _result << "startI18n()";
  }

  void Handler::endI18n()
  {
    _result << "endI18n()";
  }

}

class EcppTest : public cxxtools::unit::TestSuite
{
  public:
    EcppTest()
    : cxxtools::unit::TestSuite("ecpp-Test")
    {
      registerMethod("testPlain", *this, &EcppTest::testPlain);
      registerMethod("testCpp", *this, &EcppTest::testCpp);
      registerMethod("testCppInline", *this, &EcppTest::testCppInline);
      registerMethod("testExpression", *this, &EcppTest::testExpression);
      registerMethod("testHtmlExpression", *this, &EcppTest::testHtmlExpression);
      registerMethod("testPre", *this, &EcppTest::testPre);
      registerMethod("testStringArgs", *this, &EcppTest::testStringArgs);
      registerMethod("testTypedArgs", *this, &EcppTest::testTypedArgs);
      registerMethod("testStringVectorArgs", *this, &EcppTest::testStringVectorArgs);
      registerMethod("testTypedVectorArgs", *this, &EcppTest::testTypedVectorArgs);
      registerMethod("testAttr", *this, &EcppTest::testAttr);
      registerMethod("testApplicationScope", *this, &EcppTest::testApplicationScope);
      registerMethod("testThreadScope", *this, &EcppTest::testThreadScope);
      registerMethod("testSessionScope", *this, &EcppTest::testSessionScope);
      registerMethod("testSecureSessionScope", *this, &EcppTest::testSecureSessionScope);
      registerMethod("testRequestScope", *this, &EcppTest::testRequestScope);
      registerMethod("testScopeVar", *this, &EcppTest::testScopeVar);
      registerMethod("testScopeShared", *this, &EcppTest::testScopeShared);
      registerMethod("testScopePage", *this, &EcppTest::testScopePage);
      registerMethod("testScopeComponent", *this, &EcppTest::testScopeComponent);
    }

    void testPlain()
    {
      std::string src = "<html><body><h1>Hello World!<h1>\n</body></html>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(" + src + ")end()");
    }

    void testCpp()
    {
      std::string src0 =
        "<html><body><h1>Hello World!<h1>\n";
      std::string src1 = 
        "<%cpp>\n"
        "  const char* s[] = \"<h1>\"\n"
        "</%cpp>\n";
      std::string src2 = 
        "</body></html>";
      std::istringstream ecpp(src0 + src1 + src2);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(" + src0 + ")onCpp(\n  const char* s[] = \"<h1>\"\n)onHtml(" + src2 + ")end()");
    }

    void testCppInline()
    {
      std::string src0 =
        "<html><body><h1>Hello World!<h1>\n";
      std::string src1 = 
        "<{const char* s[] = \"<h1>\"\n}>";
      std::string src2 = 
        "</body></html>";
      std::istringstream ecpp(src0 + src1 + src2);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(" + src0 + ")onCpp(const char* s[] = \"<h1>\"\n)onHtml(" + src2 + ")end()");
    }

    void testExpression()
    {
      std::string src = "<foo><$ a+b $></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onExpression( a+b )onHtml(</foo>)end()");
    }

    void testHtmlExpression()
    {
      std::string src = "<foo><$$ a+b $></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onHtmlExpression( a+b )onHtml(</foo>)end()");
    }

    void testPre()
    {
      std::string src = "<foo><%pre> #include <iostream> </%pre></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onPre( #include <iostream> )onHtml(</foo>)end()");
    }

    void testStringArgs()
    {
      std::string src = "<foo><%args> foo=\"bar\"; </%args></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onArg(foo, \"bar\")onHtml(</foo>)end()");
    }

    void testTypedArgs()
    {
      std::string src = "<foo><%args> int foo = 17; </%args></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onArg(int foo ,  17)onHtml(</foo>)end()");
    }

    void testStringVectorArgs()
    {
      std::string src = "<foo><%args> foo[]; </%args></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onArg(foo[], )onHtml(</foo>)end()");
    }

    void testTypedVectorArgs()
    {
      std::string src = "<foo><%args> int foo[]; </%args></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onArg(int foo[], )onHtml(</foo>)end()");
    }

    void testAttr()
    {
      std::string src = "<foo><%attr> method=\"post\"; </%attr></foo>";
      std::istringstream ecpp(src);
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onAttr(method, \"post\")onHtml(</foo>)end()");
    }

    void testApplicationScope()
    {
      std::istringstream ecpp("<foo><%application>blah;</%application></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(application,component,,blah,)onHtml(</foo>)end()");
    }

    void testThreadScope()
    {
      std::istringstream ecpp("<foo><%thread>blah;</%thread></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(thread,component,,blah,)onHtml(</foo>)end()");
    }

    void testSessionScope()
    {
      std::istringstream ecpp("<foo><%session>blah;</%session></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(session,component,,blah,)onHtml(</foo>)end()");
    }

    void testSecureSessionScope()
    {
      std::istringstream ecpp("<foo><%securesession>blah;</%securesession></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(secure_session,component,,blah,)onHtml(</foo>)end()");
    }

    void testRequestScope()
    {
      std::istringstream ecpp("<foo><%request>blah;</%request></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(request,component,,blah,)onHtml(</foo>)end()");
    }

    void testScopeVar()
    {
      std::istringstream ecpp("<foo><%request>\nfoo bar=5;\n</%request></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(request,component,foo,bar,5)onHtml(</foo>)end()");
    }

    void testScopeShared()
    {
      std::istringstream ecpp("<foo><%request scope=\"shared\">\nfoo;\n</%request></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(request,shared,,foo,)onHtml(</foo>)end()");
    }

    void testScopePage()
    {
      std::istringstream ecpp("<foo><%request scope=\"page\">\nfoo;\n</%request></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(request,page,,foo,)onHtml(</foo>)end()");
    }

    void testScopeComponent()
    {
      std::istringstream ecpp("<foo><%request \n\t scope=\"component\">\nfoo;\n</%request></foo>");
      Handler handler;
      tnt::ecpp::Parser parser(handler, std::string());
      parser.parse(ecpp);
      CXXTOOLS_UNIT_ASSERT_EQUALS(handler.result(), "start()onHtml(<foo>)onScope(request,component,,foo,)onHtml(</foo>)end()");
    }

};

cxxtools::unit::RegisterTest<EcppTest> register_EcppTest;
