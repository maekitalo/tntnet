////////////////////////////////////////////////////////////////////////
// mime.cpp
//

#include <tnt/component.h>
#include <tnt/http.h>
#include <tnt/tntconfig.h>
#include <fstream>
#include <cxxtools/thread.h>
#include <tnt/log.h>

namespace tnt
{
  class urlmapper;
  class comploader;
}

static Mutex mutex;
static tnt::component* theComponent = 0;
static unsigned refs = 0;
static bool configured = false;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_mime(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
  bool config_mime(const tnt::tntconfig::name_type& key,
    const tnt::tntconfig::config_value_type& values);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
namespace tntcomp
{
  class mime : public tnt::component
  {
    public:
      static const std::string ConfigDefaultType;
      static const std::string ConfigAddType;

    private:
      typedef std::map<std::string, std::string> mime_map_type;
      static mime_map_type mime_map;
      static std::string default_type;

    protected:
      virtual ~mime() { };

    public:
      virtual unsigned operator() (tnt::httpRequest& request,
        tnt::httpReply& reply, query_params& qparam);
      virtual bool drop();

      static void setDefaultType(const std::string& type)
        { default_type = type; }
      static const std::string& getDefaultType()
        { return default_type; }
      static void addType(const std::string& type, const std::string& ext)
        { mime_map.insert(mime_map_type::value_type(ext, type)); }
  };
}

////////////////////////////////////////////////////////////////////////
// external functions
//

bool config_mime(const tnt::tntconfig::name_type& key,
  const tnt::tntconfig::config_value_type& values)
{
  if (configured)
    return false;

  if (key == tntcomp::mime::ConfigDefaultType)
  {
    if (values.size() >= 1)
    {
      if (!tntcomp::mime::getDefaultType().empty())
        log_warn_ns(tntcomp, "DefaultType already set");
      else
      {
        log_debug_ns(tntcomp, "DefaultType " << values[0]);
        tntcomp::mime::setDefaultType(values[0]);
        return true;
      }
    }
    else
    {
      log_warn_ns(tntcomp, "missing parameter in DefaultType");
    }
  }
  else if (key == tntcomp::mime::ConfigAddType)
  {
    if (values.size() >= 2)
    {
      for (tnt::tntconfig::config_value_type::size_type i = 1;
           i < values.size(); ++i)
      {
        log_debug_ns(tntcomp, "AddType \"" << values[0]
          << "\" \"" << values[i] << '"');
        tntcomp::mime::addType(values[0], values[i]);
      }
      return true;
    }
    else
    {
      log_warn_ns(tntcomp, "missing parameter in AddType");
    }
  }

  return false;
}

tnt::component* create_mime(const tnt::compident& ci,
  const tnt::urlmapper& um, tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new tntcomp::mime();
    refs = 1;
  }
  else
    ++refs;

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//

namespace tntcomp
{
  const std::string mime::ConfigDefaultType = "DefaultType";
  const std::string mime::ConfigAddType = "AddType";
  mime::mime_map_type mime::mime_map;
  std::string mime::default_type;

  unsigned mime::operator() (tnt::httpRequest& request,
    tnt::httpReply& reply, query_params& qparams)
  {
    std::string path = request.getPathInfo();

    if (request.getArgs().size() > 0)
      reply.setContentType(request.getArg(0));
    else
    {
      for (mime_map_type::const_iterator it = mime_map.begin();
           it != mime_map.end(); ++it)
      {
        std::string ext = it->first;
        if (path.size() >= ext.size()
            && path.compare(path.size() - ext.size(), ext.size(), ext) == 0)
        {
          log_debug("url-path=\"" << path << "\" type=" << it->second);
          reply.setContentType(it->second);
          return DECLINED;
        }
      }

      log_warn("unknown type in url-path \"" << path << "\" set DefaultType " << default_type);
      reply.setContentType(default_type);
    }

    // we do not produce any content, so we should pass the request
    // to the next handler:
    return DECLINED;
  }

  bool mime::drop()
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

}

