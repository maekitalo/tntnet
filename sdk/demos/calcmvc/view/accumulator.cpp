////////////////////////////////////////////////////////////////////////
// view/accumulator.cpp
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

log_define("component.view.accumulator")

// <%pre>
#include <model/accumulator.h>
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

static tnt::EcppComponentFactoryImpl<_component_> Factory("view/accumulator");

static const char* rawData = "\014\000\000\000(\000\000\000\275\000\000\000\n<h1>Accumulator</h1>\nvalue=\n\n<form meth"
  "od='post'>\n  <input type='submit' name='increment' value='increment'>\n  <input type='submit' name='decrement' value"
  "='decrement'>\n</form>\n";

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
  log_trace("view/accumulator " << qparam.getUrl());

  tnt::DataChunks data(rawData);

#line 2 "view/accumulator.ecpp"
  TNT_SESSION_GLOBAL_VAR(model::Accumulator, accumulator, ());   // <%session> model::Accumulator accumulator
  // <%cpp>
  reply.out() << data[0]; // \n<h1>Accumulator</h1>\nvalue=
#line 6 "view/accumulator.ecpp"
  reply.sout() << ( accumulator.value() );
  reply.out() << data[1]; // \n\n<form method='post'>\n  <input type='submit' name='increment' value='increment'>\n  <input type='submit' name='decrement' value='decrement'>\n</form>\n
  // <%/cpp>
  return HTTP_OK;
}

} // namespace
