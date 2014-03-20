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


#ifndef TNT_HTTP_H
#define TNT_HTTP_H

static const unsigned DECLINED = 0;
static const unsigned HTTP_CONTINUE = 100;
static const unsigned HTTP_SWITCHING_PROTOCOLS = 101;
static const unsigned HTTP_PROCESSING = 102;
static const unsigned HTTP_OK = 200;
static const unsigned HTTP_CREATED = 201;
static const unsigned HTTP_ACCEPTED = 202;
static const unsigned HTTP_NON_AUTHORITATIVE = 203;
static const unsigned HTTP_NO_CONTENT = 204;
static const unsigned HTTP_RESET_CONTENT = 205;
static const unsigned HTTP_PARTIAL_CONTENT = 206;
static const unsigned HTTP_MULTI_STATUS = 207;
static const unsigned HTTP_MULTIPLE_CHOICES = 300;
static const unsigned HTTP_MOVED_PERMANENTLY = 301;
static const unsigned HTTP_MOVED_TEMPORARILY = 302;
static const unsigned HTTP_SEE_OTHER = 303;
static const unsigned HTTP_NOT_MODIFIED = 304;
static const unsigned HTTP_USE_PROXY = 305;
static const unsigned HTTP_TEMPORARY_REDIRECT = 307;
static const unsigned HTTP_BAD_REQUEST = 400;
static const unsigned HTTP_UNAUTHORIZED = 401;
static const unsigned HTTP_PAYMENT_REQUIRED = 402;
static const unsigned HTTP_FORBIDDEN = 403;
static const unsigned HTTP_NOT_FOUND = 404;
static const unsigned HTTP_METHOD_NOT_ALLOWED = 405;
static const unsigned HTTP_NOT_ACCEPTABLE = 406;
static const unsigned HTTP_PROXY_AUTHENTICATION_REQUIRED = 407;
static const unsigned HTTP_REQUEST_TIME_OUT = 408;
static const unsigned HTTP_CONFLICT = 409;
static const unsigned HTTP_GONE = 410;
static const unsigned HTTP_LENGTH_REQUIRED = 411;
static const unsigned HTTP_PRECONDITION_FAILED = 412;
static const unsigned HTTP_REQUEST_ENTITY_TOO_LARGE = 413;
static const unsigned HTTP_REQUEST_URI_TOO_LARGE = 414;
static const unsigned HTTP_UNSUPPORTED_MEDIA_TYPE = 415;
static const unsigned HTTP_RANGE_NOT_SATISFIABLE = 416;
static const unsigned HTTP_EXPECTATION_FAILED = 417;
static const unsigned HTTP_UNPROCESSABLE_ENTITY = 422;
static const unsigned HTTP_LOCKED = 423;
static const unsigned HTTP_FAILED_DEPENDENCY = 424;
static const unsigned HTTP_UPGRADE_REQUIRED = 426;
static const unsigned HTTP_INTERNAL_SERVER_ERROR = 500;
static const unsigned HTTP_NOT_IMPLEMENTED = 501;
static const unsigned HTTP_BAD_GATEWAY = 502;
static const unsigned HTTP_SERVICE_UNAVAILABLE = 503;
static const unsigned HTTP_GATEWAY_TIME_OUT = 504;
static const unsigned HTTP_VERSION_NOT_SUPPORTED = 505;
static const unsigned HTTP_VARIANT_ALSO_VARIES = 506;
static const unsigned HTTP_INSUFFICIENT_STORAGE = 507;
static const unsigned HTTP_NOT_EXTENDED = 510;

#endif // TNT_HTTP_H

