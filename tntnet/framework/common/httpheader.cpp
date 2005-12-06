/* httpheader.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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
    const std::string acceptLanguage = "Accept-Language:";
    const std::string acceptEncoding = "Accept-Encoding:";
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
