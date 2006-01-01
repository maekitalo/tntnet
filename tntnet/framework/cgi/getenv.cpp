////////////////////////////////////////////////////////////////////////
// getenv.cpp
// generated with ecppc
// date: Sun Jan  1 16:45:19 2006
//

#include <tnt/ecpp.h>
#include <tnt/convert.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httpheader.h>
#include <tnt/http.h>
#include <tnt/data.h>
#include <tnt/componentfactory.h>
#include <tnt/componentguard.h>
#include <tnt/objecttemplate.h>
#include <tnt/objectptr.h>
#include <cxxtools/log.h>
#include <stdexcept>

log_define("component.getenv")

template <typename T> inline void use(const T&) { }

// <%pre>

#include <unistd.h>
// </%pre>

namespace component
{

// <%declare_shared>
// </%declare_shared>
class getenv : public tnt::EcppComponent
{
    friend class getenvFactory;
    getenv& main()  { return *this; }

    // <%declare>
    // </%declare>

    // <%config>
    // </%config>

  protected:
    ~getenv();

  public:
    getenv(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);

    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
    void drop();
    unsigned    getDataCount(const tnt::HttpRequest& request) const;
    unsigned    getDataLen(const tnt::HttpRequest& request, unsigned n) const;
    const char* getDataPtr(const tnt::HttpRequest& request, unsigned n) const;

};

class getenvFactory : public tnt::SingletonComponentFactory
{
  public:
    getenvFactory(const std::string& componentName)
      : tnt::SingletonComponentFactory(componentName)
      { }
    virtual tnt::Component* doCreate(const tnt::Compident& ci,
      const tnt::Urlmapper& um, tnt::Comploader& cl);
};

tnt::Component* getenvFactory::doCreate(const tnt::Compident& ci,
  const tnt::Urlmapper& um, tnt::Comploader& cl)
{
  return new getenv(ci, um, cl);
}

TNT_COMPONENTFACTORY(getenv, getenvFactory)

static tnt::RawData rawData(
"\030\000\000\000\224\000\000\000\240\000\000\000\254\000\000\000\271\000\000\000\322\000\000\000<html>\n<style type=\"text/css\">\ntable {\n  border:2px solid black;\n}\ntd {\n  border:1px solid blue;\n}\n\n</style>\n<body>\n<table>\n <tr>\n  <td></td>\n  <td></td>\n </tr>\n</table>\n</body>\n</html>\n",
  210);
static tnt::DataChunks<tnt::RawData> data(rawData);

#define DATA(dc, r, n) data[n]
#define DATA_SIZE(dc, r, n) data.size(n)

// <%shared>
// </%shared>

// <%config>
// </%config>

getenv::getenv(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
  : EcppComponent(ci, um, cl)
{
  // <%init>
  // </%init>
}

getenv::~getenv()
{
  // <%cleanup>
  // </%cleanup>
}

unsigned getenv::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, cxxtools::QueryParams& qparam)
{
  const Component* dataComponent = this;
  ::use(dataComponent);


  TNT_SESSION_COMPONENT_VAR(int, hi, "hi", ());   // <%session> int hi
  // <%args>
  // </%args>

  // <%cpp>
  reply.out() << DATA(dataComponent, request, 0); // <html>\n<style type="text/css">\ntable {\n  border:2px solid black;\n}\ntd {\n  border:1px solid blue;\n}\n\n</style>\n<body>\n<table>\n

  for (char** env = environ; *env; ++env)
  {
    std::string e = *env;
    std::string::size_type p = e.find('=');
    std::string n = e.substr(0, p);
    std::string v = e.substr(p + 1);
  reply.out() << DATA(dataComponent, request, 1); //  <tr>\n  <td>
  reply.sout() << (n);
  reply.out() << DATA(dataComponent, request, 2); // </td>\n  <td>
  reply.sout() << (v);
  reply.out() << DATA(dataComponent, request, 3); // </td>\n </tr>\n

  }
  reply.out() << DATA(dataComponent, request, 4); // </table>\n</body>\n</html>\n
  // <%/cpp>
  return HTTP_OK;
}

void getenv::drop()
{
  factory.drop(this);
}

unsigned getenv::getDataCount(const tnt::HttpRequest& request) const
{ return data.size(); }

unsigned getenv::getDataLen(const tnt::HttpRequest& request, unsigned n) const
{
  if (n >= data.size())
    throw std::range_error("range_error in getenv::getDataLen");
  return data[n].getLength();
}

const char* getenv::getDataPtr(const tnt::HttpRequest& request, unsigned n) const
{
  if (n >= data.size())
    throw std::range_error("range_error in getenv::getDataPtr");
  return data[n].getData();
}

} // namespace component
