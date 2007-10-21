/* openssl.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/openssl.h"
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
        throw OpensslException(buffer, code);
      }
      else
      {
        log_debug("unknown SSL-Error " << code);
        throw OpensslException("unknown SSL-Error", code);
      }
    }
  }

  static cxxtools::Mutex *openssl_mutex;

  static unsigned long pthreads_thread_id()
  {
    return (unsigned long)pthread_self();
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
      openssl_mutex[n].lock();
    else
      openssl_mutex[n].unlock();
  }

  static void thread_setup(void)
  {
    openssl_mutex = new cxxtools::Mutex[CRYPTO_num_locks()];

    CRYPTO_set_id_callback(pthreads_thread_id);
    CRYPTO_set_locking_callback(pthreads_locking_callback);
  }

  static cxxtools::Mutex mutex;
  static void openssl_init()
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

  void OpensslServer::installCertificates(const char* certificateFile, const char* privateKeyFile)
  {
    log_debug("use certificate file " << certificateFile);
    if (SSL_CTX_use_certificate_file(ctx, certificateFile, SSL_FILETYPE_PEM) <= 0)
      checkSslError();

    log_debug("use private key file " << privateKeyFile);
    if (SSL_CTX_use_PrivateKey_file(ctx, privateKeyFile, SSL_FILETYPE_PEM) <= 0)
      checkSslError();

    log_debug("check private key");
    if (!SSL_CTX_check_private_key(ctx))
      throw OpensslException("private key does not match the certificate public key", 0);

    log_debug("private key ok");
  }

  OpensslServer::OpensslServer(const char* certificateFile)
  {
    openssl_init();

    log_debug("SSL_CTX_new(SSLv23_server_method())");
    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, certificateFile);
  }

  OpensslServer::OpensslServer(const char* certificateFile, const char* privateKeyFile)
  {
    openssl_init();

    log_debug("SSL_CTX_new(SSLv23_server_method())");
    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, privateKeyFile);
  }

  OpensslServer::~OpensslServer()
  {
    if (ctx)
    {
      log_debug("SSL_CTX_free(ctx)");
      SSL_CTX_free(ctx);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // OpensslStream
  //
  OpensslStream::OpensslStream()
    : ssl(0)
  {
    openssl_init();
  }

  OpensslStream::OpensslStream(const OpensslServer& server)
    : ssl(0)
  {
    openssl_init();
    accept(server);
  }

  OpensslStream::~OpensslStream()
  {
    if (ssl)
    {
      try
      {
        shutdown();
      }
      catch (const std::exception& e)
      {
        log_error("error closing ssl-connection: " << e.what());
      }

      log_debug("SSL_free(" << ssl << ')');
      SSL_free(ssl);
    }
  }

  void OpensslStream::accept(const OpensslServer& server)
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

  int OpensslStream::sslRead(char* buffer, int bufsize) const
  {
    // I had crashes without this (and the lock in sslWrite) lock:
    // openssl should be thread-safe, with the installed callbacks, but I did not
    // get it working
    cxxtools::MutexLock lock(mutex);

    log_debug("read");

    int n;
    int err;

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("SSL_read(" << ssl << ", buffer, " << bufsize << ") (blocking)");
        n = ::SSL_read(ssl, buffer, bufsize);
      } while (n <= 0 &&
                ((err = SSL_get_error(ssl, n)) == SSL_ERROR_WANT_READ
               || err == SSL_ERROR_WANT_WRITE));
      checkSslError();
    }
    else
    {
      // non-blocking/with timeout

      // try read
      log_debug("SSL_read(" << ssl << ", buffer, " << bufsize << ')');
      n = ::SSL_read(ssl, buffer, bufsize);
      log_debug("ssl-read => " << n);

      if (n > 0)
        return n;

      log_debug("SSL_get_error(" << ssl << ", " << n << ')');
      if ((err = SSL_get_error(ssl, n)) != SSL_ERROR_WANT_READ
       && err != SSL_ERROR_WANT_WRITE)
        checkSslError();

      if (getTimeout() == 0)
      {
        log_debug("read-timeout");
        throw cxxtools::net::Timeout();
      }

      // no read, timeout > 0 - poll
      do
      {
        short events;

        if (SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE)
        {
          log_debug("poll(POLLIN|POLLOUT)");
          events = POLLIN|POLLOUT;
        }
        else
        {
          log_debug("poll(POLLIN)");
          events = POLLIN;
        }

        poll(events);

        log_debug("SSL_read(" << ssl << ", buffer, " << bufsize << ')');
        n = ::SSL_read(ssl, buffer, bufsize);
        log_debug("SSL_read returns " << n);
        checkSslError();

      } while (n < 0
         && ((err = SSL_get_error(ssl, n)) == SSL_ERROR_WANT_READ
          || err == SSL_ERROR_WANT_WRITE
          || SSL_get_error(ssl, n) == SSL_ERROR_SYSCALL && errno == EAGAIN));
    }
    return n;
  }

  int OpensslStream::sslWrite(const char* buffer, int bufsize) const
  {
    // I had crashes without this (and the lock in sslRead) lock:
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

      poll(SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE
                  ? POLLIN|POLLOUT : POLLIN);
    }

    log_debug("OpensslStream::sslWrite returns " << bufsize);
    return bufsize;
  }

  void OpensslStream::shutdown() const
  {
    cxxtools::MutexLock lock(mutex);

    int n, err;
    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("shutdown unbuffered");
        n = ::SSL_shutdown(ssl);
      } while (n <= 0 &&
                ((err = SSL_get_error(ssl, n)) == SSL_ERROR_WANT_READ
              || err == SSL_ERROR_WANT_WRITE));
      checkSslError();
    }
    else
    {
      // non-blocking/with timeout

      // try read
      log_debug("SSL_shutdown(" << ssl << ')');
      n = ::SSL_shutdown(ssl);
      log_debug("ssl-shutdown => " << n);

      log_debug("SSL_get_error(" << ssl << ", " << n << ')');
      if ((err = SSL_get_error(ssl, n)) != SSL_ERROR_WANT_READ
       && err != SSL_ERROR_WANT_WRITE)
        checkSslError();

      if (getTimeout() == 0)
      {
        log_debug("shutdown-timeout");
        throw cxxtools::net::Timeout();
      }

      do
      {
        log_debug("poll");
        poll(err == SSL_ERROR_WANT_WRITE ? POLLIN|POLLOUT : POLLIN);

        log_debug("SSL_shutdown(" << ssl << ')');
        n = ::SSL_shutdown(ssl);
        log_debug("SSL_shutdown returns " << n);

        checkSslError();

      } while (n <= 0
         && ((err = SSL_get_error(ssl, n)) == SSL_ERROR_WANT_READ
          || err == SSL_ERROR_WANT_WRITE));
    }
  }

  //////////////////////////////////////////////////////////////////////
  // openssl_streambuf
  //
  openssl_streambuf::openssl_streambuf(OpensslStream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_buffer(new char_type[bufsize]),
      m_bufsize(bufsize)
  {
    setTimeout(timeout);
  }

  openssl_streambuf::int_type openssl_streambuf::overflow(openssl_streambuf::int_type c)
  {
    try
    {
      if (pptr() != pbase())
      {
        int n = m_stream.sslWrite(pbase(), pptr() - pbase());
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
      log_error("error int openssl_streambuf::overflow: " << e.what());
      return traits_type::eof();
    }
  }

  openssl_streambuf::int_type openssl_streambuf::underflow()
  {
    int n = m_stream.sslRead(m_buffer, m_bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer, m_buffer, m_buffer + n);
    return (int_type)(unsigned char)m_buffer[0];
  }

  int openssl_streambuf::sync()
  {
    try
    {
      if (pptr() != pbase())
      {
        int n = m_stream.sslWrite(pbase(), pptr() - pbase());
        if (n <= 0)
          return -1;
        else
          setp(m_buffer, m_buffer + m_bufsize);
      }
      return 0;
    }
    catch (const std::exception& e)
    {
      log_error("error in openssl_streambuf::sync(): " << e.what());
      return traits_type::eof();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // openssl_iostream
  //
}
