/* mime.cpp
   Copyright (C) 2003 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <tnt/component.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/tntconfig.h>
#include <tnt/comploader.h>
#include <fstream>
#include <cxxtools/thread.h>
#include <cxxtools/log.h>

log_define("tntnet.mime")

namespace tnt
{
  class Urlmapper;
}

static cxxtools::Mutex mutex;
static tnt::Component* theComponent = 0;
static unsigned refs = 0;
static bool configured = false;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::Component* create_mime(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
namespace tntcomp
{
  class Mime : public tnt::Component
  {
    public:
      static const std::string ConfigDefaultType;
      static const std::string ConfigAddType;

    private:
      typedef std::map<std::string, std::string> mime_map_type;
      static mime_map_type mime_map;
      static std::string default_type;

    protected:
      virtual ~Mime() { };

    public:
      virtual unsigned operator() (tnt::HttpRequest& request,
        tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
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

class MimeConfigurator
{
  public:
    void operator() (const tnt::Tntconfig::config_entry_type& entry)
    {
      if (entry.key == tntcomp::Mime::ConfigDefaultType)
      {
        if (entry.params.size() >= 1)
        {
          if (!tntcomp::Mime::getDefaultType().empty())
            log_warn("DefaultType already set");
          else
          {
            log_debug("DefaultType " << entry.params[0]);
            tntcomp::Mime::setDefaultType(entry.params[0]);
          }
        }
        else
        {
          log_warn("missing parameter in DefaultType");
        }
      }
      else if (entry.key == tntcomp::Mime::ConfigAddType)
      {
        if (entry.params.size() >= 2)
        {
          for (tnt::Tntconfig::params_type::size_type i = 1;
               i < entry.params.size(); ++i)
          {
            log_debug("AddType \"" << entry.params[0]
              << "\" \"" << entry.params[i] << '"');
            tntcomp::Mime::addType(entry.params[0], entry.params[i]);
          }
        }
        else
        {
          log_warn("missing parameter in AddType");
        }
      }
    }
};

tnt::Component* create_mime(const tnt::Compident& ci,
  const tnt::Urlmapper& um, tnt::Comploader& cl)
{
  cxxtools::MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new tntcomp::Mime();
    refs = 1;

    if (!configured)
    {
      const tnt::Tntconfig& config = cl.getConfig();
      std::for_each(config.getConfigValues().begin(),
                    config.getConfigValues().end(),
                    MimeConfigurator());
      configured = true;
    }
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
  const std::string Mime::ConfigDefaultType = "DefaultType";
  const std::string Mime::ConfigAddType = "AddType";
  Mime::mime_map_type Mime::mime_map;
  std::string Mime::default_type;

  unsigned Mime::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
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
          log_info("content-type " << it->second);
          reply.setContentType(it->second);
          return DECLINED;
        }
      }

      log_warn("unknown type in url-path \"" << path << "\" set DefaultType " << default_type);
      log_info("content-type " << default_type);
      reply.setContentType(default_type);
    }

    // we do not produce any content, so we should pass the request
    // to the next handler:
    return DECLINED;
  }

  bool Mime::drop()
  {
    cxxtools::MutexLock lock(mutex);
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

