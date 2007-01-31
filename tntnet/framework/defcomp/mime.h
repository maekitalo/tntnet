/* mime.h
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNT_MIME_H
#define TNT_MIME_H

#include <tnt/tntconfig.h>
#include <tnt/mimedb.h>
#include <tnt/component.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // MimeBase
  //
  class MimeBase
  {
    public:
      static const std::string ConfigDefaultType;
      static const std::string ConfigAddType;

    private:
      typedef tnt::MimeDb mime_map_type;
      static mime_map_type mime_map;
      static std::string default_type;
      static bool configured;

    public:
      static void setDefaultType(const std::string& type)
        { default_type = type; }
      static const std::string& getDefaultType()
        { return default_type; }
      static void addType(const std::string& type, const std::string& ext)
        { mime_map.insert(mime_map_type::value_type(ext, type)); }
      static void readMimeDb(const std::string& file)
        { mime_map.read(file); }
      static void doConfigure(const std::string& mimeDb, const tnt::Tntconfig& config);

      static std::string getMimeType(const std::string& path);
  };

  ////////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Mime : public tnt::Component, public MimeBase
  {
      friend class MimeFactory;

    public:
      Mime()
      { }

      virtual unsigned operator() (tnt::HttpRequest& request,
        tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
      virtual void drop();
  };

}

#endif // TNT_MIME_H
