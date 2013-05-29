////////////////////////////////////////////////////////////////////////
// view/calc.cpp
// generated with ecppc
//

#include <tnt/ecpp.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httpheader.h>
#include <tnt/http.h>
#include <tnt/data.h>
#include <tnt/componentfactory.h>
#include <cxxtools/log.h>
#include <stdexcept>

log_define("component.view.calc")

// <%pre>
#include <model/calc.h>
// </%pre>

namespace
{
class _component_ : public tnt::EcppComponent
{
    _component_& main()  { return *this; }

  protected:
    ~_component_();

  public:
    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);

    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

static tnt::EcppComponentFactoryImpl<_component_> Factory("view/calc");

static const char* rawData = "(\000\000\000f\000\000\000\220\000\000\000\301\000\000\000\210\001\000\000\222\001\000"
  "\000\223\001\000\000\224\001\000\000\227\001\000\000\231\001\000\000  <h1>Tommi's Tnt-Calculator</h1>\n\n  <form meth"
  "od='post'>\n\n   \n   <input type=\"text\" name=\"arg1\" value=\"\"> <br>\n   <input type=\"text\" name=\"arg2\" valu"
  "e=\"\"> <br>\n   <input type=\"submit\" name=\"op\" value=\"+\">\n   <input type=\"submit\" name=\"op\" value=\"-\">"
  "\n   <input type=\"submit\" name=\"op\" value=\"*\">\n   <input type=\"submit\" name=\"op\" value=\"/\">\n  </form>\n"
  "\n\n  <hr>\n     = \n\n";

// <%shared>
// </%shared>

// <%config>
// </%config>

#define SET_LANG(lang) \
     do \
     { \
       request.setLang(lang); \
       reply.setLocale(request.getLocale()); \
     } while (false)

_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
  : EcppComponent(ci, um, cl)
{
  // <%init>
  // </%init>
}

_component_::~_component_()
{
  // <%cleanup>
  // </%cleanup>
}

unsigned _component_::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
{
  log_trace("view/calc " << qparam.getUrl());

  tnt::DataChunks data(rawData);

#line 2 "view/calc.ecpp"
  TNT_REQUEST_GLOBAL_VAR(model::Calc, calc, ());   // <%request> model::Calc calc
  // <%cpp>
  reply.out() << data[0]; //   <h1>Tommi's Tnt-Calculator</h1>\n\n  <form method='post'>\n\n   
  reply.out() << data[1]; // \n   <input type="text" name="arg1" value="
#line 10 "view/calc.ecpp"
  reply.sout() << ( calc.arg1() );
  reply.out() << data[2]; // "> <br>\n   <input type="text" name="arg2" value="
#line 11 "view/calc.ecpp"
  reply.sout() << ( calc.arg2() );
  reply.out() << data[3]; // "> <br>\n   <input type="submit" name="op" value="+">\n   <input type="submit" name="op" value="-">\n   <input type="submit" name="op" value="*">\n   <input type="submit" name="op" value="/">\n  </form>\n\n
#line 18 "view/calc.ecpp"
 if (calc.resultOk()) {

  reply.out() << data[4]; // \n  <hr>\n  
#line 21 "view/calc.ecpp"
  reply.sout() << ( calc.arg1() );
  reply.out() << data[5]; //  
#line 21 "view/calc.ecpp"
  reply.sout() << ( calc.op() );
  reply.out() << data[6]; //  
#line 21 "view/calc.ecpp"
  reply.sout() << ( calc.arg2() );
  reply.out() << data[7]; //  = 
#line 21 "view/calc.ecpp"
  reply.sout() << ( calc.result() );
  reply.out() << data[8]; // \n\n
#line 23 "view/calc.ecpp"
 }

  // <%/cpp>
  return HTTP_OK;
}

} // namespace
