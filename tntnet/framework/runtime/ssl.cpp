/* ssl.cpp
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

#include "tnt/ssl.h"
#include <cxxtools/thread.h>
#include <openssl/err.h>
#include <cxxtools/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

log_define("tntnet.ssl");

namespace tnt
{
  static void checkSslError()
  {
    unsigned long code = ERR_get_error();
    if (code != 0)
    {
      char buffer[120];
      if (ERR_error_string(code, buffer))
        throw SslException(buffer, code);
      else
        throw SslException("unknown SSL-Error", code);
    }
  }

  static void ssl_init()
  {
    static bool initialized = false;
    static cxxtools::Mutex mutex;

    if (!initialized)
    {
      cxxtools::MutexLock lock(mutex);
      if (!initialized)
      {
        SSL_load_error_strings();
        SSL_library_init();
        checkSslError();
        initialized = true;
      }
    }
  }

  void SslServer::installCertificates(const char* certificateFile, const char* privateKeyFile)
  {
    log_debug("use certificate file " << certificateFile);
    if (SSL_CTX_use_certificate_file(ctx, certificateFile, SSL_FILETYPE_PEM) <= 0)
      checkSslError();

    log_debug("use private key file " << privateKeyFile);
    if (SSL_CTX_use_PrivateKey_file(ctx, privateKeyFile, SSL_FILETYPE_PEM) <= 0)
      checkSslError();

    log_debug("check private key");
    if (!SSL_CTX_check_private_key(ctx))
      throw SslException("private key does not match the certificate public key", 0);

    log_debug("private key ok");
  }

  SslServer::SslServer(const char* certificateFile)
  {
    ssl_init();

    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, certificateFile);
  }

  SslServer::SslServer(const char* certificateFile, const char* privateKeyFile)
  {
    ssl_init();

    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, privateKeyFile);
  }

  SslServer::~SslServer()
  {
    if (ctx)
      SSL_CTX_free(ctx);
  }

  //////////////////////////////////////////////////////////////////////
  // SslStream
  //
  SslStream::SslStream()
    : ssl(0)
  {
    ssl_init();
  }

  SslStream::SslStream(const SslServer& server)
    : ssl(0)
  {
    ssl_init();

    ssl = SSL_new( server.getSslContext() );
    checkSslError();

    SSL_set_accept_state(ssl);
    SSL_set_fd(ssl, getFd());

    SSL_accept(ssl);
    checkSslError();
  }

  SslStream::~SslStream()
  {
    if (ssl)
      SSL_free(ssl);
  }

  void SslStream::Accept(const SslServer& server)
  {
    log_debug("accept");
    Stream::Accept(server);

    log_debug("tcp-connection established - build ssltunnel");
    ssl = SSL_new( server.getSslContext() );
    checkSslError();

    SSL_set_fd(ssl, getFd());

    SSL_set_accept_state(ssl);
    SSL_accept(ssl);
    checkSslError();

    setTimeout(getTimeout());

    log_debug("ssl-connection ready");
  }

  int SslStream::SslRead(char* buffer, int bufsize) const
  {
    log_debug("read");

    int n;

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("read unbuffered");
        n = ::SSL_read(ssl, buffer, bufsize);
      } while (n <= 0 &&
                (SSL_get_error(ssl, n) == SSL_ERROR_WANT_READ
              || SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE));
    }
    else
    {
      // non-blocking/with timeout

      // try read
      n = ::SSL_read(ssl, buffer, bufsize);

      log_debug("ssl-read => " << n);

      if (n > 0)
        return n;

      if (SSL_get_error(ssl, n) != SSL_ERROR_WANT_READ
       && SSL_get_error(ssl, n) != SSL_ERROR_WANT_WRITE)
        checkSslError();

      if (getTimeout() == 0)
      {
        log_warn("timeout");
        throw cxxtools::tcp::Timeout();
      }

      // no read, timeout > 0 - poll
      do
      {
        log_debug("poll");

        struct pollfd fds;
        fds.fd = getFd();
        fds.events =
          (SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE)
            ? POLLIN|POLLOUT
            : POLLIN;
        int p = ::poll(&fds, 1, /*getTimeout()*/2000);

        log_debug("poll => " << p << " revents=" << fds.revents);

        if (p <= 0)
        {
          // no data
          log_warn("timeout");
          throw cxxtools::tcp::Timeout();
        }

        n = ::SSL_read(ssl, buffer, bufsize);
        log_debug("SSL_read returns " << n);

      } while (n <= 0
         && (SSL_get_error(ssl, n) == SSL_ERROR_WANT_READ
          || SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE));
    }

    checkSslError();
    return n;
  }

  int SslStream::SslWrite(const char* buffer, int bufsize) const
  {
    int count = SSL_write(ssl, buffer, bufsize);
    checkSslError();
    return count;
  }

  //////////////////////////////////////////////////////////////////////
  // ssl_streambuf
  //
  ssl_streambuf::ssl_streambuf(SslStream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_buffer(new char_type[bufsize]),
      m_bufsize(bufsize)
  {
    setTimeout(timeout);
  }

  ssl_streambuf::int_type ssl_streambuf::overflow(ssl_streambuf::int_type c)
  {
    if (pptr() != pbase())
    {
      int n = m_stream.SslWrite(pbase(), pptr() - pbase());
      if (n <= 0)
        return traits_type::eof();
    }

    setp(m_buffer, m_buffer + m_bufsize);
    if (c != traits_type::eof())
    {
      *pptr() = (char_type)c;
      pbump(1);
    }

    return 0;
  }

  ssl_streambuf::int_type ssl_streambuf::underflow()
  {
    int n = m_stream.SslRead(m_buffer, m_bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer, m_buffer, m_buffer + n);
    return (int_type)(unsigned char)m_buffer[0];
  }

  int ssl_streambuf::sync()
  {
    if (pptr() != pbase())
    {
      int n = m_stream.SslWrite(pbase(), pptr() - pbase());
      if (n <= 0)
        return -1;
      else
        setp(m_buffer, m_buffer + m_bufsize);
    }
    return 0;
  }

  //////////////////////////////////////////////////////////////////////
  // ssl_iostream
  //
}
