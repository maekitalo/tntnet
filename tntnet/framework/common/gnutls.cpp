/* gnutls.cpp
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


#include "tnt/gnutls.h"
#include "tnt/tntnet.h"
#include <cxxtools/thread.h>
#include <cxxtools/log.h>
#include <sstream>
#include "tnt/gcryptinit.h"
#include <sys/poll.h>
#include <errno.h>

log_define("tntnet.ssl")

namespace tnt
{
  namespace
  {
    cxxtools::Mutex mutex;
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsException
  //
  std::string GnuTlsException::formatMessage(const std::string& function,
    int code)
  {
    std::ostringstream msg;
    msg << "error " << code << " in function " << function
        << ": " << gnutls_strerror(code);
    return msg.str();
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsInit
  //

  unsigned GnuTlsInit::initCount = 0;
  gnutls_dh_params_t GnuTlsInit::dhParams;

  GnuTlsInit::GnuTlsInit()
  {
    int ret;

    cxxtools::MutexLock lock(mutex);
    if (initCount++ == 0)
    {
      log_debug("gcry_control");
      ret = gcrypt_init();
      if (ret != 0)
        throw GnuTlsException("gcry_control", ret);

      log_debug("gnutls_global_init()");
      ret = gnutls_global_init();
      if (ret != 0)
        throw GnuTlsException("gnutls_global_init", ret);

      log_debug("gnutls_dh_params_init(dhParams)");
      ret = gnutls_dh_params_init(&dhParams);
      if (ret != 0)
        throw GnuTlsException("gnutls_dh_params_init", ret);

      log_debug("gnutls_dh_params_generate2(dh_params, 1024)");
      ret = gnutls_dh_params_generate2(dhParams, 1024);
      if (ret != 0)
        throw GnuTlsException("gnutls_dh_params_generate2", ret);
    }
  }

  GnuTlsInit::~GnuTlsInit()
  {
    cxxtools::MutexLock lock(mutex);
    if (--initCount == 0)
    {
      log_debug("gnutls_global_deinit()");
      gnutls_global_deinit();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsX509Cred
  //
  GnuTlsX509Cred::GnuTlsX509Cred(const char* certificateFile,
    const char* privateKeyFile)
  {
    int ret;

    log_debug("gnutls_certificate_allocate_credentials");
    ret = gnutls_certificate_allocate_credentials(&x509_cred);
    if (ret != 0)
      throw GnuTlsException("gnutls_certificate_allocate_credentials", ret);
    log_debug("gnutls_certificate_allocate_credentials => " << x509_cred);

    // gnutls_certificate_set_x509_trust_file(x509_cred, caFile, GNUTLS_X509_FMT_PEM);
    // gnutls_certificate_set_x509_crl_file(x509_cred, crlFile, GNUTLS_X509_FMT_PEM);

    try
    {
      log_debug("gnutls_certificate_set_x509_key_file(x509_cred, \""
        << certificateFile << "\", \"" << privateKeyFile
        << "\", GNUTLS_X509_FMT_PEM)");
      ret = gnutls_certificate_set_x509_key_file(x509_cred, certificateFile,
        privateKeyFile, GNUTLS_X509_FMT_PEM);
      if (ret < 0)
        throw GnuTlsException("gnutls_certificate_set_x509_key_file", ret);

      log_debug("gnutls_certificate_set_dh_params");
      gnutls_certificate_set_dh_params(x509_cred, init.getDhParams());
    }
    catch (...)
    {
      log_debug("gnutls_certificate_free_credentials");
      gnutls_certificate_free_credentials(x509_cred);
      throw;
    }
  }

  GnuTlsX509Cred::~GnuTlsX509Cred()
  {
    log_debug("gnutls_certificate_free_credentials");
    gnutls_certificate_free_credentials(x509_cred);
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsServer
  //

  GnuTlsServer::GnuTlsServer(const char* certificateFile, const char* privateKeyFile)
    : cred(certificateFile, privateKeyFile)
  {
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsStream
  //

  GnuTlsStream::~GnuTlsStream()
  {
    if (session)
    {
      try
      {
        shutdown();
      }
      catch (const std::exception& e)
      {
        log_error("error shutting down ssl-conneciton: " << e.what());
      }

      log_debug("gnutls_deinit(session)");
      gnutls_deinit(session);
    }
  }

  void GnuTlsStream::accept(const GnuTlsServer& server)
  {
    log_debug("accept");
    cxxtools::net::Stream::accept(server);

    if (Tntnet::shouldStop())
      return;

    log_debug("gnutls_init(session, GNUTLS_SERVER)");
    int ret = gnutls_init(&session, GNUTLS_SERVER);
    if (ret != 0)
      throw GnuTlsException("gnutls_init", ret);

    log_debug("gnutls_set_default_priority");
    ret = gnutls_set_default_priority(session);
    if (ret != 0)
      throw GnuTlsException("gnutls_set_default_priority", ret);

    log_debug("gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, "
      << server.getCred() << ')');
    ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, server.getCred());
    if (ret != 0)
      throw GnuTlsException("gnutls_credentials_set", ret);

    log_debug("gnutls_dh_set_prime_bits(session, 1024)");
    gnutls_dh_set_prime_bits(session, 1024);

    log_debug("gnutls_transport_set_ptr(" << getFd() << ')');
    gnutls_transport_set_ptr(session, (gnutls_transport_ptr_t)getFd());

    log_debug("gnutls_transport_set_lowat(0)");
    gnutls_transport_set_lowat(session, 0);

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("gnutls_handshake");
        ret = gnutls_handshake(session);
      } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);
    }
    else
    {
      // non-blocking/with timeout

      while (true)
      {
        log_debug("gnutls_handshake");
        ret = gnutls_handshake(session);
        log_debug("gnutls_handshake => " << ret);

        if (ret == 0)
          break;

        if (ret != GNUTLS_E_INTERRUPTED && ret != GNUTLS_E_AGAIN)
          throw GnuTlsException("gnutls_handshake", ret);

        log_debug("poll");
        if (poll(POLLIN) & POLLHUP)
        {
          log_error("eof in gnutls_handshake");
          throw std::runtime_error("eof in gnutls_handshake");
        }
      }
    }

    log_debug("ssl-handshake was completed");
  }

  int GnuTlsStream::sslRead(char* buffer, int bufsize) const
  {
    int ret;

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("gnutls_record_recv");
        ret = gnutls_record_recv(session, buffer, bufsize);
      } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);
    }
    else
    {
      // non-blocking/with timeout

      while (true)
      {
        log_debug("gnutls_record_recv (" << bufsize << ')');
        ret = gnutls_record_recv(session, buffer, bufsize);
        log_debug("gnutls_record_recv => " << ret);

        if (ret >= 0)
          break;

        if (ret < 0 && ret != GNUTLS_E_INTERRUPTED && ret != GNUTLS_E_AGAIN)
          throw GnuTlsException("gnutls_record_recv", ret);

        log_debug("poll");
        if (poll(POLLIN) & POLLHUP)
        {
          log_debug("eof in read");
          return ret;
        }
      }
    }

    return ret;
  }

  int GnuTlsStream::sslWrite(const char* buffer, int bufsize) const
  {
    int ret;

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("gnutls_record_send");
        ret = gnutls_record_send(session, buffer, bufsize);
      } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);
    }
    else
    {
      // non-blocking/with timeout

      while (true)
      {
        log_debug("gnutls_record_send");
        ret = gnutls_record_send(session, buffer, bufsize);
        log_debug("gnutls_record_send => " << ret);

        if (ret > 0)
          break;

        if (ret != GNUTLS_E_INTERRUPTED && ret != GNUTLS_E_AGAIN)
          throw GnuTlsException("gnutls_record_send", ret);

        log_debug("poll");
        if (poll(POLLIN|POLLOUT) & POLLHUP)
        {
          log_debug("eof in write");
          return ret;
        }
      }
    }

    return ret;
  }

  void GnuTlsStream::shutdown() const
  {
    int ret;

    if (getTimeout() < 0)
    {
      // blocking
      do
      {
        log_debug("gnutls_bye");
        ret = gnutls_bye(session, GNUTLS_SHUT_RDWR);
      } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);
    }
    else
    {
      // non-blocking/with timeout

      while (true)
      {
        log_debug("gnutls_bye");
        ret = gnutls_bye(session, GNUTLS_SHUT_RDWR);
        log_debug("gnutls_bye => " << ret);

        if (ret == 0)
          break;

        if (ret < 0
          && ret != GNUTLS_E_INTERRUPTED
          && ret != GNUTLS_E_AGAIN)
            throw GnuTlsException("gnutls_bye", ret);

        log_debug("poll");
        poll(POLLIN);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTls_streambuf
  //
  GnuTls_streambuf::GnuTls_streambuf(GnuTlsStream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_buffer(new char_type[bufsize]),
      m_bufsize(bufsize)
  {
    setTimeout(timeout);
  }

  GnuTls_streambuf::int_type GnuTls_streambuf::overflow(GnuTls_streambuf::int_type c)
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
      log_error("error int GnuTls_streambuf::overflow: " << e.what());
      return traits_type::eof();
    }
  }

  GnuTls_streambuf::int_type GnuTls_streambuf::underflow()
  {
    int n = m_stream.sslRead(m_buffer, m_bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer, m_buffer, m_buffer + n);
    return (int_type)(unsigned char)m_buffer[0];
  }

  int GnuTls_streambuf::sync()
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
      log_error("error in GnuTls_streambuf::sync(): " << e.what());
      return traits_type::eof();
    }
  }

}
