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

log_define("tntnet.ssl")

namespace tnt
{
  static void checkSslError()
  {
    log_debug("ERR_get_error");
    unsigned long code = ERR_get_error();
    if (code != 0)
    {
      char buffer[120];
      log_debug("ERR_error_string");
      if (ERR_error_string(code, buffer))
      {
        log_debug("SSL-Error " << code << ": \"" << buffer << '"');
        throw SslException(buffer, code);
      }
      else
      {
        log_debug("unknown SSL-Error " << code);
        throw SslException("unknown SSL-Error", code);
      }
    }
  }

  static cxxtools::Mutex *ssl_mutex;

  static unsigned long pthreads_thread_id()
  {
    return static_cast<unsigned long>(pthread_self());
  }

  static void pthreads_locking_callback(int mode, int n, const char *file,
    int line)
  {
    /*
    log_debug("pthreads_locking_callback " << CRYPTO_thread_id()
      << " n=" << n
      << " mode=" << ((mode & CRYPTO_LOCK) ? 'l' : 'u')
      << ' ' << file << ':' << line); */

    if (mode & CRYPTO_LOCK)
      ssl_mutex[n].lock();
    else
      ssl_mutex[n].unlock();
  }

  static void thread_setup(void)
  {
    ssl_mutex = new cxxtools::Mutex[CRYPTO_num_locks()];

    CRYPTO_set_id_callback(pthreads_thread_id);
    CRYPTO_set_locking_callback(pthreads_locking_callback);
  }

  static cxxtools::Mutex mutex;
  static void ssl_init()
  {
    static bool initialized = false;

    if (!initialized)
    {
      cxxtools::MutexLock lock(mutex);
      if (!initialized)
      {
        log_debug("SSL_load_error_strings");
        SSL_load_error_strings();
        log_debug("SSL_library_init");
        SSL_library_init();

        checkSslError();
        thread_setup();
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

    log_debug("SSL_CTX_new(SSLv23_server_method())");
    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, certificateFile);
  }

  SslServer::SslServer(const char* certificateFile, const char* privateKeyFile)
  {
    ssl_init();

    log_debug("SSL_CTX_new(SSLv23_server_method())");
    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, privateKeyFile);
  }

  SslServer::~SslServer()
  {
    if (ctx)
    {
      log_debug("SSL_CTX_free(ctx)");
      SSL_CTX_free(ctx);
    }
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
    accept(server);
  }

  SslStream::~SslStream()
  {
    if (ssl)
    {
      log_debug("SSL_free(" << ssl << ')');
      SSL_free(ssl);
    }
  }

  void SslStream::accept(const SslServer& server)
  {
    log_debug("accept");
    Stream::accept(server);

    log_debug("tcp-connection established - build ssltunnel");

    log_debug("SSL_new(" << server.getSslContext() << ')');
    ssl = SSL_new( server.getSslContext() );
    checkSslError();

    log_debug("SSL_set_fd(" << ssl << ", " << getFd() << ')');
    SSL_set_fd(ssl, getFd());

    log_debug("SSL_set_accept_state(" << ssl << ')');
    SSL_set_accept_state(ssl);
  }

  int SslStream::SslRead(char* buffer, int bufsize) const
  {
    // I had crashes without this (and the lock in SslWrite) lock:
    // openssl should be thread-safe, with the installed callbacks, but I did not
    // get it working
    cxxtools::MutexLock lock(mutex);

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
      log_debug("SSL_read");
      n = ::SSL_read(ssl, buffer, bufsize);

      log_debug("ssl-read => " << n);

      if (n > 0)
        return n;

      log_debug("SSL_get_error(" << ssl << ", " << n << ')');
      if (SSL_get_error(ssl, n) != SSL_ERROR_WANT_READ
       && SSL_get_error(ssl, n) != SSL_ERROR_WANT_WRITE)
        checkSslError();

      if (getTimeout() == 0)
      {
        log_debug("read-timeout");
        throw cxxtools::net::Timeout();
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
        int p = ::poll(&fds, 1, getTimeout());

        log_debug("poll => " << p << " revents=" << fds.revents);

        if (p < 0)
        {
          int errnum = errno;
          throw cxxtools::net::Exception(strerror(errnum));
        }
        else if (p == 0)
        {
          // no data
          log_debug("read-timeout");
          throw cxxtools::net::Timeout();
        }

        log_debug("SSL_read(" << ssl << ", buffer, " << bufsize << ')');
        n = ::SSL_read(ssl, buffer, bufsize);
        log_debug("SSL_read returns " << n);
        checkSslError();

      } while (n <= 0
         && (SSL_get_error(ssl, n) == SSL_ERROR_WANT_READ
          || SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE));
    }

    checkSslError();
    return n;
  }

  int SslStream::SslWrite(const char* buffer, int bufsize) const
  {
    // I had crashes without this (and the lock in SslRead) lock:
    // openssl should be thread-safe, with the installed callbacks, but I did not
    // get it working
    cxxtools::MutexLock lock(mutex);

    int n = 0;
    int s = bufsize;

    while (true)
    {
      log_debug("SSL_write(" << ssl << ", buffer, " << s << ')');
      n = SSL_write(ssl, buffer, s);
      checkSslError();

      if (n > 0)
      {
        buffer += n;
        s -= n;
      }

      if (s <= 0)
        break;

      struct pollfd fds;
      fds.fd = getFd();
      fds.events =
        (SSL_get_error(ssl, n) == SSL_ERROR_WANT_READ)
          ? POLLIN|POLLOUT : POLLOUT;
      int p = ::poll(&fds, 1, getTimeout());

      if (p < 0)
      {
        int errnum = errno;
        throw cxxtools::net::Exception(strerror(errnum));
      }
      else if (p == 0)
      {
        // no data
        log_warn("write-timeout");
        throw cxxtools::net::Timeout();
      }
    }

    log_debug("SslStream::SslWrite returns " << bufsize);
    return bufsize;
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
    try
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
    catch (const std::exception& e)
    {
      log_error("error int ssl_streambuf::overflow: " << e.what());
      return traits_type::eof();
    }
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
    try
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
    catch (const std::exception& e)
    {
      log_error("error in ssl_streambuf::sync(): " << e.what());
      return traits_type::eof();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // ssl_iostream
  //
}
