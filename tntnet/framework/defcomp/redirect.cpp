////////////////////////////////////////////////////////////////////////
// redirect.cpp
//

#include <tnt/component.h>
#include <tnt/http.h>
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
  tnt::component* create_redirect(const tnt::compident& ci, const tnt::urlmapper& um,
    tnt::comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class redirectcomp : public tnt::component
{
  protected:
    virtual ~redirectcomp() { };

  public:
    virtual unsigned operator() (const tnt::httpRequest& request,
      tnt::httpReply& reply, query_params& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_redirect(const tnt::compident& ci, const tnt::urlmapper& um,
  tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new redirectcomp();
    refs = 1;
  }

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//
unsigned redirectcomp::operator() (const tnt::httpRequest& request,
  tnt::httpReply& reply, query_params&)
{
  return reply.redirect(request.getPathInfo());
}

bool redirectcomp::drop()
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

