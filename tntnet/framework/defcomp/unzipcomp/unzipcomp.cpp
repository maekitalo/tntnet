////////////////////////////////////////////////////////////////////////
// unzipcomp.cpp
//

#include "unzip.h"

#include <tnt/component.h>
#include <tnt/http.h>
#include <fstream>
#include <cxxtools/thread.h>
#include "unzip_pp.h"

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
  tnt::component* create_unzip(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class unzipcomp : public tnt::component
{
    static std::string document_root;

  protected:
    virtual ~unzipcomp() { };

  public:
    virtual unsigned operator() (tnt::httpRequest& request,
      tnt::httpReply& reply, query_params& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_unzip(const tnt::compident& ci,
  const tnt::urlmapper& um, tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new unzipcomp();
    refs = 1;
  }
  else
    ++refs;

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//

unsigned unzipcomp::operator() (tnt::httpRequest& request,
  tnt::httpReply& reply, query_params& qparams)
{
  std::string pi = request.getPathInfo();

  if (request.getArgsCount() < 1)
    reply.throwError(HTTP_INTERNAL_SERVER_ERROR, "missing archive name");

  unzipFile f(request.getArg(0));
  unzipFileStream in(f, pi, false);

  std::copy(std::istreambuf_iterator<char>(in),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(reply.out()));

  return HTTP_OK;
}

bool unzipcomp::drop()
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

