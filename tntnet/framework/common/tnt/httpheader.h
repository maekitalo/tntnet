/* tnt/httpheader.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#ifndef TNT_HTTPHEADER_H
#define TNT_HTTPHEADER_H

#include <string>

namespace tnt
{
  namespace httpheader
  {
    extern const std::string contentType;
    extern const std::string contentLength;
    extern const std::string connection;
    extern const std::string connectionClose;
    extern const std::string connectionKeepAlive;
    extern const std::string lastModified;
    extern const std::string server;
    extern const std::string serverName;
    extern const std::string location;
    extern const std::string accept;
    extern const std::string acceptLanguage;
    extern const std::string acceptEncoding;
    extern const std::string acceptCharset;
    extern const std::string contentEncoding;
    extern const std::string date;
    extern const std::string keepAlive;
    extern const std::string ifModifiedSince;
    extern const std::string host;
    extern const std::string cacheControl;
    extern const std::string contentMD5;
    extern const std::string setCookie;
    extern const std::string cookie;
    extern const std::string pragma;
    extern const std::string expires;
    extern const std::string userAgent;
  }
}

#endif // TNT_HTTPHEADER_H
