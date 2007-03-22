/* httpheader.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "tnt/httpheader.h"
#include "config.h"

namespace tnt
{
  namespace httpheader
  {
    const std::string contentType = "Content-Type:";
    const std::string contentLength = "Content-Length:";
    const std::string connection = "Connection:";
    const std::string connectionClose = "close";
    const std::string connectionKeepAlive = "Keep-Alive";
    const std::string lastModified = "Last-Modified:";
    const std::string server = "Server:";
    const std::string serverName = "Tntnet/" VERSION;
    const std::string location = "Location:";
    const std::string accept = "Accept:";
    const std::string acceptLanguage = "Accept-Language:";
    const std::string acceptEncoding = "Accept-Encoding:";
    const std::string acceptCharset = "Accept-Charset:";
    const std::string contentEncoding = "Content-Encoding:";
    const std::string date = "Date:";
    const std::string keepAlive = "Keep-Alive:";
    const std::string ifModifiedSince = "If-Modified-Since:";
    const std::string host = "Host:";
    const std::string cacheControl = "Cache-Control:";
    const std::string contentMD5 = "Content-MD5:";
    const std::string setCookie = "Set-Cookie:";
    const std::string cookie = "Cookie:";
    const std::string pragma = "Pragma:";
    const std::string expires = "Expires:";
    const std::string userAgent = "User-Agent:";
  }
}
