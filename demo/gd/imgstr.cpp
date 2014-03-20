////////////////////////////////////////////////////////////////////////
// imgstr.cpp
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

log_define("component.imgstr")

// <%pre>
#line 9 "imgstr.ecpp"


#include "gd++.h"


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

static tnt::EcppComponentFactoryImpl<_component_> Factory("imgstr");

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
  log_trace("imgstr " << qparam.getUrl());


  // <%args>
std::string str = qparam.param("str", ( "Hello"));
unsigned x = qparam.arg<unsigned>("x", ( 200));
unsigned y = qparam.arg<unsigned>("y", ( 20));
int red = qparam.arg<int>("red", ( 255));
int green = qparam.arg<int>("green", ( 0));
int blue = qparam.arg<int>("blue", ( 0));
  // </%args>

  // <%cpp>
#line 14 "imgstr.ecpp"


  // create image
  gd::Gd image(x, y);

  // allocate background color (background color is the first allocated color in gd)
  int white = image.colorAllocate(255, 255, 255);

  // allocate color for string
  int color = image.colorAllocate(red, green, blue);

  // write text to image
  image.drawString(0, 0, str.c_str(), color);

  // output image
  reply.setContentType("image/png");
  reply.out() << image;


  // <%/cpp>
  return HTTP_OK;
}

} // namespace
