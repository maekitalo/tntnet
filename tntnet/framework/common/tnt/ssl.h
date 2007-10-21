/* tnt/ssl.h
 * Copyright (C) 2006 Tommi Maekitalo
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


#ifndef TNT_SSL_H
#define TNT_SSL_H

#include "config.h"

#ifdef WITH_GNUTLS
#  include "tnt/gnutls.h"
#  define USE_SSL
#endif

#ifdef WITH_OPENSSL
#  include "tnt/openssl.h"
#  define USE_SSL
#endif

namespace tnt
{
#ifdef WITH_GNUTLS

  typedef GnuTlsServer SslServer;
  typedef GnuTls_iostream ssl_iostream;

#endif

#ifdef WITH_OPENSSL

  typedef OpensslServer SslServer;
  typedef openssl_iostream ssl_iostream;

#endif

}

#endif // TNT_SSL_H

