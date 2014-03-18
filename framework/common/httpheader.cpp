/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/httpheader.h"

namespace tnt
{
  namespace httpheader
  {
    const char* contentType = "Content-Type:";
    const char* contentLength = "Content-Length:";
    const char* connection = "Connection:";
    const char* connectionClose = "close";
    const char* connectionKeepAlive = "Keep-Alive";
    const char* lastModified = "Last-Modified:";
    const char* server = "Server:";
    const char* location = "Location:";
    const char* accept = "Accept:";
    const char* acceptLanguage = "Accept-Language:";
    const char* acceptEncoding = "Accept-Encoding:";
    const char* acceptCharset = "Accept-Charset:";
    const char* acceptRanges = "Accept-Ranges:";
    const char* contentEncoding = "Content-Encoding:";
    const char* date = "Date:";
    const char* keepAlive = "Keep-Alive:";
    const char* ifModifiedSince = "If-Modified-Since:";
    const char* host = "Host:";
    const char* cacheControl = "Cache-Control:";
    const char* contentMD5 = "Content-MD5:";
    const char* setCookie = "Set-Cookie:";
    const char* cookie = "Cookie:";
    const char* pragma = "Pragma:";
    const char* expires = "Expires:";
    const char* userAgent = "User-Agent:";
    const char* wwwAuthenticate = "WWW-Authenticate:";
    const char* authorization = "Authorization:";
    const char* referer = "Referer:";
    const char* range = "Range:";
    const char* contentRange = "Content-Range:";
    const char* contentLocation = "Content-Location:";
    const char* contentDisposition = "Content-Disposition:";
    const char* age = "Age:";
    const char* transferEncoding = "Transfer-Encoding:";
  }
}

