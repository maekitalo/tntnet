////////////////////////////////////////////////////////////////////////
// static.cpp
//

#include <tnt/component.h>
#include <tnt/http.h>
#include <tnt/tntconfig.h>
#include <fstream>
#include <cxxtools/thread.h>

namespace tnt
{
  class urlmapper;
  class comploader;
}

static Mutex mutex;
static tnt::component* theComponent = 0;
static unsigned refs = 0;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_static(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
  bool config(const tnt::tntconfig::name_type& key,
    const tnt::tntconfig::config_value_type& values);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class staticcomp : public tnt::component
{
    static std::string document_root;

  protected:
    virtual ~staticcomp() { };

  public:
    virtual unsigned operator() (const tnt::httpRequest& request,
      tnt::httpReply& reply, query_params& qparam);
    virtual bool drop();

    static void setDocumentRoot(const std::string& s)
    { document_root = s; }
    static const std::string& getDocumentRoot()
    { return document_root; }
};

////////////////////////////////////////////////////////////////////////
// external functions
//

bool config(const tnt::tntconfig::name_type& key,
  const tnt::tntconfig::config_value_type& values)
{
  if (key == "DocumentRoot" && values.size() >= 1)
  {
    staticcomp::setDocumentRoot(values[0]);
    return true;
  }
  return false;
}

tnt::component* create_static(const tnt::compident& ci,
  const tnt::urlmapper& um, tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new staticcomp();
    refs = 1;
  }
  else
    ++refs;

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//

std::string staticcomp::document_root = "htdocs";

unsigned staticcomp::operator() (const tnt::httpRequest& request,
  tnt::httpReply& reply, query_params& qparams)
{
  std::string file = document_root + '/' + request.getPathInfo();
  std::ifstream in(file.c_str());

  if (request.getArgs().size() > 0)
    reply.setContentType(request.getArg(0));

  if (!in)
    reply.throwNotFound(request.getPathInfo());

  reply.setDirectMode();
  reply.out() << in.rdbuf();

  return HTTP_OK;
}

bool staticcomp::drop()
{
  MutexLock lock(mutex);
  if (--refs == 0)
  {
    delete this;
    theComponent = 0;
    return true;
  }
  else
    return false;
}

