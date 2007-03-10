/* mimehandler.cpp
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

#include <mimehandler.h>
#include <cxxtools/log.h>

log_define("tntnet.mime")

namespace tnt
{
  const std::string MimeHandler::configMimeDb = "MimeDb";
  const std::string MimeHandler::configDefaultType = "DefaultType";
  const std::string MimeHandler::configAddType = "AddType";

  MimeHandler::MimeHandler(const tnt::Tntconfig& config)
    : default_type(config.getValue(configDefaultType, "text/html"))
  {
    mime_map.read(config.getValue(configMimeDb, "/etc/mime.types"));

    for (tnt::Tntconfig::config_entries_type::const_iterator it = config.getConfigValues().begin();
         it != config.getConfigValues().end(); ++it)
    {
      if (it->key == configAddType)
      {
        for (tnt::Tntconfig::params_type::size_type i = 1;
             i < it->params.size(); ++i)
        {
          log_debug("AddType \"" << it->params[0] << "\" \"" << it->params[i] << '"');
          mime_map.insert(mime_map_type::value_type(it->params[0], it->params[i]));
        }
      }
    }
  }

  std::string MimeHandler::getMimeType(const std::string& path) const
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
        return it->second;
      }
    }

    log_debug("unknown type in url-path \"" << path << "\" set DefaultType " << default_type);
    return default_type;
  }

}
