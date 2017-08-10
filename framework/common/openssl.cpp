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


#include "tnt/openssl.h"
#include "tnt/tntnet.h"
#include <cxxtools/mutex.h>
#include <openssl/err.h>
#include <cxxtools/log.h>
#include <cxxtools/ioerror.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <pthread.h>
#include <tnt/tntconfig.h>

using namespace std;

log_define("tntnet.ssl")

namespace tnt
{
  namespace
  {
    void throwOpensslException(const char* what, unsigned long code)
    {
      throw OpensslException(what, code);
    }

    void checkSslError()
    {
      unsigned long code = ERR_get_error();
      if (code != 0)
      {
        char buffer[120];
        if (ERR_error_string(code, buffer))
        {
          log_debug("SSL-Error " << code << ": \"" << buffer << '"');
          throwOpensslException(buffer, code);
        }
        else
        {
          log_debug("unknown SSL-Error " << code);
          throwOpensslException("unknown SSL-Error", code);
        }
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
    if (SSL_CTX_use_certificate_chain_file(ctx.getPointer(), certificateFile) <= 0)
      checkSslError();

    log_debug("use private key file " << privateKeyFile);
    if (SSL_CTX_use_PrivateKey_file(ctx.getPointer(), privateKeyFile, SSL_FILETYPE_PEM) <= 0)
      checkSslError();

    log_debug("check private key");
    if (!SSL_CTX_check_private_key(ctx.getPointer()))
      throwOpensslException("private key does not match the certificate public key", 0);

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

    //restrict protocols
    string sslProtocols=TntConfig::it().sslProtocols;
    if(sslProtocols.find("+SSLv3") != string::npos){
        SSL_CTX_clear_options(ctx.getPointer(), SSL_OP_NO_SSLv3);
    }
    if(sslProtocols.find("-SSLv3") != string::npos){
        SSL_CTX_set_options(ctx.getPointer(), SSL_OP_NO_SSLv3);
    }
    if(sslProtocols.find("+TLSv1_0") != string::npos){
        SSL_CTX_clear_options(ctx.getPointer(), SSL_OP_NO_TLSv1);
    }
    if(sslProtocols.find("-TLSv1_0")!= string::npos){
        SSL_CTX_set_options(ctx.getPointer(), SSL_OP_NO_TLSv1);
    }
    if(sslProtocols.find("+TLSv1_1") != string::npos){
        SSL_CTX_clear_options(ctx.getPointer(), SSL_OP_NO_TLSv1_1);
    }
    if(sslProtocols.find("-TLSv1_1") != string::npos){
        SSL_CTX_set_options(ctx.getPointer(), SSL_OP_NO_TLSv1_1);
    }
    if(sslProtocols.find("+TLSv1_2") != string::npos){
        SSL_CTX_clear_options(ctx.getPointer(), SSL_OP_NO_TLSv1_2);
    }
    if(sslProtocols.find("-TLSv1_2") != string::npos){
        SSL_CTX_set_options(ctx.getPointer(), SSL_OP_NO_TLSv1_2);
    }


    //restrict cipher list
    if(!TntConfig::it().sslCipherList.empty()){
        int cipher_lst = SSL_CTX_set_cipher_list(ctx.getPointer(),
                TntConfig::it().sslCipherList.c_str());
        if(! cipher_lst ) {
            log_error( "SSL_CTX_set_cipher_list");
        }
    }

    /* ANSI X9.62 Prime 256v1 curve */
    EC_KEY *ecdh = EC_KEY_new_by_curve_name (NID_X9_62_prime256v1);
    if (! ecdh){
        log_error("curve prime256v1 not supported");
    }else {
        log_debug("SSL_CTX_set_tmp_ecdh");
        SSL_CTX_set_tmp_ecdh (ctx.getPointer(), ecdh);
        EC_KEY_free (ecdh);
    }


    installCertificates(certificateFile, privateKeyFile);
  }

  void SslCtxReleaser<SSL_CTX>::destroy(SSL_CTX* ctx)
  {
    log_debug("SSL_CTX_free(ctx)");
    SSL_CTX_free(ctx);
  }

  //////////////////////////////////////////////////////////////////////
  // OpensslStream
  //
  OpensslStream::OpensslStream()
    : ssl(0)
  {
    openssl_init();
  }

  OpensslStream::OpensslStream(const OpensslServer& server, bool inherit)
    : ctx(server.getSslContext()),
      ssl(0)
  {
    openssl_init();
    accept(server, inherit);
  }

  OpensslStream::~OpensslStream()
  {
    if (ssl && !Tntnet::shouldStop())
    {
      try
      {
        shutdown();
      }
      catch (const std::exception& e)
      {
        log_debug("error shutting down ssl-conneciton: " << e.what());
      }

      SSL_free(ssl);
    }
  }

  void OpensslStream::accept(const OpensslServer& server, bool inherit)
  {
    log_trace("accept");
    cxxtools::net::TcpSocket::accept(server, inherit);
  }

  void OpensslStream::handshake(const OpensslServer& server)
  {
    log_debug("tcp-connection established - build ssltunnel");

    log_debug("SSL_new(" << server.getSslContext().getPointer() << ')');
    ssl = SSL_new( server.getSslContext().getPointer() );
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
      throw cxxtools::IOTimeout();
    }

    // no read, timeout > 0 - poll
    do
    {
      poll(SSL_get_error(ssl, n) == SSL_ERROR_WANT_WRITE
                  ? POLLIN|POLLOUT : POLLIN);

      log_debug("SSL_read(" << ssl << ", buffer, " << bufsize << ')');
      n = ::SSL_read(ssl, buffer, bufsize);
      log_debug("SSL_read returns " << n);
      checkSslError();

    } while (n < 0
       && ((err = SSL_get_error(ssl, n)) == SSL_ERROR_WANT_READ
        || err == SSL_ERROR_WANT_WRITE
        || (err == SSL_ERROR_SYSCALL && errno == EAGAIN)));

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

      int err = SSL_ERROR_WANT_WRITE;
      if (n > 0)
      {
        buffer += n;
        s -= n;
      }
      else if (n < 0
              && (err = SSL_get_error(ssl, n)) != SSL_ERROR_WANT_READ
              && err != SSL_ERROR_WANT_WRITE
              && (err != SSL_ERROR_SYSCALL || errno != EAGAIN))
      {
        log_debug("error " << err << " occured in SSL_write; n=" << n);
        throwOpensslException("error from TLS/SSL I/O operation", err);
      }

      if (s <= 0)
        break;

      log_debug("poll with " << (err == SSL_ERROR_WANT_READ ? "POLLIN" : "POLLIN|POLLOUT"));
      poll(err == SSL_ERROR_WANT_READ ? POLLIN : POLLIN|POLLOUT);
    }

    log_debug("OpensslStream::sslWrite returns " << bufsize);
    return bufsize;
  }

  void OpensslStream::shutdown() const
  {
    cxxtools::MutexLock lock(mutex);

    int n, err;
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
      throw cxxtools::IOTimeout();
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
