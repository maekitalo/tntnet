////////////////////////////////////////////////////////////////////////
// error.cpp
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
  tnt::component* create_error(const tnt::compident& ci, const tnt::urlmapper& um,
    tnt::comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class errorcomp : public tnt::component
{
  protected:
    virtual ~errorcomp() { };

  public:
    virtual unsigned operator() (const tnt::httpRequest& request,
      tnt::httpReply& reply, query_params& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_error(const tnt::compident& ci, const tnt::urlmapper& um,
  tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new errorcomp();
    refs = 1;
  }

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//
unsigned errorcomp::operator() (const tnt::httpRequest& request,
  tnt::httpReply& reply, query_params&)
{
  std::string msg;

  const tnt::httpRequest::args_type& args = request.getArgs();

  tnt::httpRequest::args_type::const_iterator i = args.begin();
  if (i == args.end())
    reply.throwError("400 internal error");

  msg = *i++;
  for ( ; i != args.end(); ++i)
  {
    msg += ' ';
    msg += *i;
  }

  reply.throwError(msg);

  return DECLINED;
}

bool errorcomp::drop()
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

