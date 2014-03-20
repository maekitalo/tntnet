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


#ifndef TNT_HTTPHEADER_H
#define TNT_HTTPHEADER_H

#include <string>

namespace tnt
{
  namespace httpheader
  {
    extern const char* contentType;
    extern const char* contentLength;
    extern const char* connection;
    extern const char* connectionClose;
    extern const char* connectionKeepAlive;
    extern const char* lastModified;
    extern const char* server;
    extern const char* location;
    extern const char* accept;
    extern const char* acceptLanguage;
    extern const char* acceptEncoding;
    extern const char* acceptCharset;
    extern const char* acceptRanges;
    extern const char* contentEncoding;
    extern const char* date;
    extern const char* keepAlive;
    extern const char* ifModifiedSince;
    extern const char* host;
    extern const char* cacheControl;
    extern const char* contentMD5;
    extern const char* setCookie;
    extern const char* cookie;
    extern const char* pragma;
    extern const char* expires;
    extern const char* userAgent;
    extern const char* wwwAuthenticate;
    extern const char* authorization;
    extern const char* referer;
    extern const char* range;
    extern const char* contentRange;
    extern const char* contentLocation;
    extern const char* contentDisposition;
    extern const char* age;
    extern const char* transferEncoding;
  }
}

#endif // TNT_HTTPHEADER_H

