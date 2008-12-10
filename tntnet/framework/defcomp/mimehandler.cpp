/*
 * Copyright (C) 2003,2007 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <mimehandler.h>
#include <cxxtools/log.h>

log_define("tntnet.mime.handler")

namespace tnt
{
  const std::string MimeHandler::configMimeDb = "MimeDb";
  const std::string MimeHandler::configDefaultContentType = "DefaultContentType";
  const std::string MimeHandler::configAddType = "AddType";

  MimeHandler::MimeHandler(const tnt::Tntconfig& config)
    : defaultType(config.getValue(configDefaultContentType, "text/html"))
  {
    mimeDb.read(config.getValue(configMimeDb, "/etc/mime.types"));

    for (tnt::Tntconfig::config_entries_type::const_iterator it = config.getConfigValues().begin();
         it != config.getConfigValues().end(); ++it)
    {
      if (it->key == configAddType)
      {
        std::string type = it->params[0];
        for (tnt::Tntconfig::params_type::size_type i = 1;
             i < it->params.size(); ++i)
        {
          std::string ext = it->params[i];
          if (ext.size() > 0)
          {
            log_debug("AddType \"" << type << "\" \"" << ext << '"');
            mimeDb.addType(ext, type);
          }
        }
      }
    }
  }

  std::string MimeHandler::getMimeType(const std::string& path) const
  {
    std::string mimeType = mimeDb.getMimetype(path);
    if (mimeType.empty())
    {
      log_debug("unknown type in url-path \"" << path << "\" set DefaultContentType " << defaultType);
      return defaultType;
    }
    else
    {
      log_debug("url-path=\"" << path << "\" type=" << mimeType);
      return mimeType;
    }
  }

}
