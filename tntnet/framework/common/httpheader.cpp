/* httpheader.cpp
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
