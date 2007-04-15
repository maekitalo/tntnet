/* mimehandler.h
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef TNT_MIMEHANDLER_H
#define TNT_MIMEHANDLER_H

#include <tnt/tntconfig.h>
#include <tnt/mimedb.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // MimeHandler
  //
  class MimeHandler
  {
    public:
      static const std::string configMimeDb;
      static const std::string configDefaultType;
      static const std::string configAddType;

    private:
      typedef tnt::MimeDb mime_map_type;
      mime_map_type mime_map;
      std::string default_type;

    public:
      MimeHandler(const tnt::Tntconfig& config);

      std::string getMimeType(const std::string& path) const;
  };

}

#endif // TNT_MIMEHANDLER_H
