/* mime.cpp
 * Copyright (C) 2003,2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <mime.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/comploader.h>
#include <cxxtools/log.h>

log_define("tntnet.mime")

namespace tnt
{
  class Urlmapper;
  class Comploader;

  ////////////////////////////////////////////////////////////////////////
  // configurator-functor
  //
  class MimeConfigurator
  {
    public:
      void operator() (const tnt::Tntconfig::config_entry_type& entry);
  };

  void MimeConfigurator::operator() (const tnt::Tntconfig::config_entry_type& entry)
  {
    if (entry.key == Mime::ConfigDefaultType)
    {
      if (entry.params.size() >= 1)
      {
        if (!MimeBase::getDefaultType().empty())
          log_debug("DefaultType already set");
        else
        {
          log_debug("DefaultType " << entry.params[0]);
          MimeBase::setDefaultType(entry.params[0]);
        }
      }
      else
      {
        log_warn("missing parameter in DefaultType");
      }
    }
    else if (entry.key == MimeBase::ConfigAddType)
    {
      if (entry.params.size() >= 2)
      {
        for (tnt::Tntconfig::params_type::size_type i = 1;
             i < entry.params.size(); ++i)
        {
          log_debug("AddType \"" << entry.params[0]
            << "\" \"" << entry.params[i] << '"');
          MimeBase::addType(entry.params[0], entry.params[i]);
        }
      }
      else
      {
        log_warn("missing parameter in AddType");
      }
    }
  }

  void MimeBase::doConfigure(const std::string& mimeDb, const tnt::Tntconfig& config)
  {
    if (configured)
      return;

    Mime::readMimeDb(mimeDb);
    std::for_each(config.getConfigValues().begin(),
                  config.getConfigValues().end(),
                  MimeConfigurator());

    configured = true;
  }

  std::string MimeBase::getMimeType(const std::string& path)
  {
    for (mime_map_type::const_iterator it = mime_map.begin();
         it != mime_map.end(); ++it)
    {
      std::string ext = it->first;
      if (path.size() > ext.size()
          && (path.at(path.size() - ext.size() - 1) == '.'
            || ext.size() > 0 && ext.at(0) == '.')
          && path.compare(path.size() - ext.size(), ext.size(), ext) == 0)
      {
        log_debug("url-path=\"" << path << "\" type=" << it->second);
        log_debug("content-type " << it->second);
        return it->second;
      }
    }

    log_debug("unknown type in url-path \"" << path << "\" set DefaultType " << default_type);
    return default_type;
  }

  const std::string MimeBase::ConfigDefaultType = "DefaultType";
  const std::string MimeBase::ConfigAddType = "AddType";
  MimeBase::mime_map_type MimeBase::mime_map;
  std::string MimeBase::default_type = "text/html";
  bool MimeBase::configured = false;

  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class MimeFactory : public tnt::SingletonComponentFactory
  {
    public:
      MimeFactory(const std::string& componentName)
        : tnt::SingletonComponentFactory(componentName)
        { }
      virtual tnt::Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
      virtual void doConfigure(const tnt::Tntconfig& config);
  };

  tnt::Component* MimeFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Mime();
  }

  void MimeFactory::doConfigure(const tnt::Tntconfig& config)
  {
    std::string mimeDb = config.getValue("MimeDb", "/etc/mime.types");
    MimeBase::doConfigure(mimeDb, config);
  }

  TNT_COMPONENTFACTORY(mime, MimeFactory)

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //

  unsigned Mime::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
  {
    if (request.getArgs().size() > 0)
      reply.setContentType(request.getArg(0));
    else
      reply.setContentType(getMimeType(request.getPathInfo()));

    // we do not produce any content, so we pass the request
    // to the next handler:
    return DECLINED;
  }

  void Mime::drop()
  {
    factory.drop(this);
  }
}

