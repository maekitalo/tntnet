////////////////////////////////////////////////////////////////////////
// view/counter.cpp
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

log_define("component.view.counter")

// <%pre>
#include <counter.h>
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

static tnt::EcppComponentFactoryImpl<_component_> Factory("view/counter");

static const char* rawData = "\014\000\000\000^\000\000\000\025\001\000\000<!DOCTYPE html>\n<html>\n  <head>\n  </head>"
  "\n  <body>\n    <h1>Counter</h1>\n    value=\n\n    <form method='post'>\n      <input type='submit' name='increment'"
  " value='increment'>\n      <input type='submit' name='decrement' value='decrement'>\n    </form>\n  </body>\n</html>"
  "\n";

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
  log_trace("view/counter " << qparam.getUrl());

  tnt::DataChunks data(rawData);

#line 2 "view/counter.ecpp"
  TNT_SESSION_GLOBAL_VAR(Counter, counter, ());   // <%session> Counter counter
  // <%cpp>
  reply.out() << data[0]; // <!DOCTYPE html>\n<html>\n  <head>\n  </head>\n  <body>\n    <h1>Counter</h1>\n    value=
#line 10 "view/counter.ecpp"
  reply.sout() << ( counter.value() );
  reply.out() << data[1]; // \n\n    <form method='post'>\n      <input type='submit' name='increment' value='increment'>\n      <input type='submit' name='decrement' value='decrement'>\n    </form>\n  </body>\n</html>\n
  // <%/cpp>
  return HTTP_OK;
}

} // namespace
