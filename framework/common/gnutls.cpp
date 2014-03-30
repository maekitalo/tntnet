/*
 * Copyright (C) 2006 Tommi Maekitalo
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


#include "tnt/gnutls.h"
#include "tnt/tntnet.h"
#include <cxxtools/mutex.h>
#include <cxxtools/log.h>
#include <unistd.h>
#include <sstream>
#include <sys/poll.h>
#include <errno.h>
#include <cxxtools/ioerror.h>

log_define("tntnet.ssl")

namespace tnt
{
  namespace
  {
    cxxtools::Mutex mutex;

    // read with timeout; assume non-blocking of underlying fd
    ssize_t pull_func(gnutls_transport_ptr_t ptr, void* buffer, size_t bufsize)
    {
      const GnuTlsStream::FdInfo& fdInfo = *static_cast<const GnuTlsStream::FdInfo*>(ptr);

      while (true)
      {
        log_debug("read fd=" << fdInfo.fd << "; size=" << bufsize);
        ssize_t n = ::read(fdInfo.fd, buffer, bufsize);
        log_debug("read returns " << n << " errno=" << errno);

        if (n > 0)
          return n;
        else if (n == 0 || errno == ECONNRESET)
          return 0;
        else if (errno == EAGAIN)
        {
          // poll
          struct pollfd fds;
          fds.fd = fdInfo.fd;
          fds.events = POLLIN;

          log_debug("poll; timeout " << fdInfo.timeout);
          int p = ::poll(&fds, 1, fdInfo.timeout);
          if (p == 0)
          {
            errno = EAGAIN;
            return -1;
          }
          else if (p < 0)
            return p;
        }
        else if (errno != EINTR)
          return -1;
      }

      return -1;
    }

    // write with timeout; assume non-blocking of underlying fd
    ssize_t push_func(gnutls_transport_ptr_t ptr, const void* buffer, size_t bufsize)
    {
      const GnuTlsStream::FdInfo& fdInfo = *static_cast<const GnuTlsStream::FdInfo*>(ptr);

      while (true)
      {
        log_debug("write fd=" << fdInfo.fd << "; size=" << bufsize);
        ssize_t n = ::write(fdInfo.fd, buffer, bufsize);
        log_debug("write returns " << n);

        if (n > 0)
          return n;
        else if (n == 0 || errno == ECONNRESET)
          return 0;
        else if (errno == EAGAIN)
        {
          // poll
          struct pollfd fds;
          fds.fd = fdInfo.fd;
          fds.events = POLLOUT;

          int p = ::poll(&fds, 1, fdInfo.timeout);
          if (p == 0)
          {
            errno = EAGAIN;
            return -1;
          }
          else if (p < 0)
            return p;
        }
        else if (errno != EINTR)
          return -1;
      }

      return -1;
    }

  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsException
  //
  std::string GnuTlsException::formatMessage(const char* function,
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

  unsigned GnuTlsInit::_initCount = 0;
  gnutls_dh_params_t GnuTlsInit::_dhParams;

  GnuTlsInit::GnuTlsInit()
  {
    int ret;

    cxxtools::MutexLock lock(mutex);
    if (_initCount++ == 0)
    {
      log_debug("gnutls_global_init()");
      ret = gnutls_global_init();
      if (ret != 0)
        throw GnuTlsException("gnutls_global_init", ret);

      log_debug("gnutls_dh_params_init(dhParams)");
      ret = gnutls_dh_params_init(&_dhParams);
      if (ret != 0)
        throw GnuTlsException("gnutls_dh_params_init", ret);

      log_debug("gnutls_dh_params_generate2(dh_params, 1024)");
      ret = gnutls_dh_params_generate2(_dhParams, 1024);
      if (ret != 0)
        throw GnuTlsException("gnutls_dh_params_generate2", ret);
    }
  }

  GnuTlsInit::~GnuTlsInit()
  {
    cxxtools::MutexLock lock(mutex);
    if (--_initCount == 0)
    {
      log_debug("gnutls_global_deinit()");
      gnutls_global_deinit();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsX509Cred
  //
  GnuTlsX509Cred::GnuTlsX509Cred(const char* certificateFile, const char* privateKeyFile)
  {
    int ret;

    log_debug("gnutls_certificate_allocate_credentials");
    ret = gnutls_certificate_allocate_credentials(&_x509_cred);
    if (ret != 0)
      throw GnuTlsException("gnutls_certificate_allocate_credentials", ret);
    log_debug("gnutls_certificate_allocate_credentials => " << _x509_cred);

    // gnutls_certificate_set_x509_trust_file(x509_cred, caFile, GNUTLS_X509_FMT_PEM);
    // gnutls_certificate_set_x509_crl_file(x509_cred, crlFile, GNUTLS_X509_FMT_PEM);

    try
    {
      log_debug("gnutls_certificate_set_x509_key_file(x509_cred, \""
        << certificateFile << "\", \"" << privateKeyFile
        << "\", GNUTLS_X509_FMT_PEM)");
      ret = gnutls_certificate_set_x509_key_file(_x509_cred, certificateFile,
        privateKeyFile, GNUTLS_X509_FMT_PEM);
      if (ret < 0)
        throw GnuTlsException("gnutls_certificate_set_x509_key_file", ret);

      log_debug("gnutls_certificate_set_dh_params");
      gnutls_certificate_set_dh_params(_x509_cred, _init.getDhParams());
    }
    catch (...)
    {
      log_debug("gnutls_certificate_free_credentials");
      gnutls_certificate_free_credentials(_x509_cred);
      throw;
    }
  }

  GnuTlsX509Cred::~GnuTlsX509Cred()
  {
    log_debug("gnutls_certificate_free_credentials");
    gnutls_certificate_free_credentials(_x509_cred);
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsServer
  //

  GnuTlsServer::GnuTlsServer(const char* certificateFile, const char* privateKeyFile)
    : _cred(certificateFile, privateKeyFile)
    { }

  //////////////////////////////////////////////////////////////////////
  // GnuTlsStream
  //

  GnuTlsStream::~GnuTlsStream()
  {
    if (_session)
    {
      if (_connected)
      {
        try
        {
          shutdown();
        }
        catch (const std::exception& e)
        {
          log_debug("error shutting down ssl-conneciton: " << e.what());
        }
      }

      log_debug("gnutls_deinit(session)");
      gnutls_deinit(_session);
    }
  }

  void GnuTlsStream::accept(const GnuTlsServer& server, bool inherit)
  {
    log_debug("accept");
    cxxtools::net::TcpSocket::accept(server, inherit);
  }

  void GnuTlsStream::handshake(const GnuTlsServer& server)
  {
    log_debug("gnutls_init(session, GNUTLS_SERVER)");
    int ret = gnutls_init(&_session, GNUTLS_SERVER);
    if (ret != 0)
      throw GnuTlsException("gnutls_init", ret);

    log_debug("gnutls_set_default_priority");
    ret = gnutls_set_default_priority(_session);
    if (ret != 0)
      throw GnuTlsException("gnutls_set_default_priority", ret);

    log_debug("gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, "
      << server.getCred() << ')');
    ret = gnutls_credentials_set(_session, GNUTLS_CRD_CERTIFICATE, server.getCred());
    if (ret != 0)
      throw GnuTlsException("gnutls_credentials_set", ret);

    log_debug("gnutls_dh_set_prime_bits(session, 1024)");
    gnutls_dh_set_prime_bits(_session, 1024);

    _fdInfo.fd = getFd();
    _fdInfo.timeout = getTimeout();

    log_debug("gnutls_transport_set_ptr(ptr)");
    gnutls_transport_set_ptr(_session, static_cast<gnutls_transport_ptr_t>(&_fdInfo));

    log_debug("gnutls_transport_set_pull_function()");
    gnutls_transport_set_pull_function(_session, pull_func);

    log_debug("gnutls_transport_set_push_function()");
    gnutls_transport_set_push_function(_session, push_func);

    // non-blocking/with timeout

    _fdInfo.timeout = 10000;

    log_debug("gnutls_handshake");
    ret = gnutls_handshake(_session);
    log_debug("gnutls_handshake => " << ret);

    if (ret != 0)
      throw GnuTlsException("gnutls_handshake", ret);

    _connected = true;
    _fdInfo.timeout = getTimeout();

    log_debug("ssl-handshake was completed");
  }

  int GnuTlsStream::sslRead(char* buffer, int bufsize) const
  {
    int ret;

    _fdInfo.timeout = getTimeout();

    // non-blocking/with timeout

    while (true)
    {
        log_debug("gnutls_record_recv (" << bufsize << ')');
        ret = gnutls_record_recv(_session, buffer, bufsize);
        log_debug("gnutls_record_recv => " << ret);

        // report GNUTLS_E_REHANDSHAKE as eof
        if (ret == GNUTLS_E_REHANDSHAKE)
          return 0;

        if (ret >= 0)
          break;

        if (ret == GNUTLS_E_AGAIN)
          throw cxxtools::IOTimeout();

        if (ret < 0 && ret != GNUTLS_E_INTERRUPTED)
          throw GnuTlsException("gnutls_record_recv", ret);
    }

    return ret;
  }

  int GnuTlsStream::sslWrite(const char* buffer, int bufsize) const
  {
    int ret;

    _fdInfo.timeout = getTimeout();

    // non-blocking/with timeout

    while (true)
    {
      log_debug("gnutls_record_send");
      ret = gnutls_record_send(_session, buffer, bufsize);
      log_debug("gnutls_record_send => " << ret);

      if (ret > 0)
        break;

      if (ret == GNUTLS_E_AGAIN)
        throw cxxtools::IOTimeout();

      if (ret != GNUTLS_E_INTERRUPTED)
        throw GnuTlsException("gnutls_record_send", ret);
    }

    return ret;
  }

  void GnuTlsStream::shutdown()
  {
    int ret;

    _fdInfo.timeout = 1000;

    // non-blocking/with timeout

    while (true)
    {
      log_debug("gnutls_bye");
      ret = gnutls_bye(_session, GNUTLS_SHUT_RDWR);
      log_debug("gnutls_bye => " << ret);

      if (ret == 0)
      {
        _connected = false;
        break;
      }

      if (ret < 0
        && ret != GNUTLS_E_INTERRUPTED
        && ret != GNUTLS_E_AGAIN)
          throw GnuTlsException("gnutls_bye", ret);

      log_debug("poll");
      poll(POLLIN);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // GnuTls_streambuf
  //
  GnuTls_streambuf::GnuTls_streambuf(GnuTlsStream& stream, unsigned bufsize, int timeout)
    : _stream(stream),
      _buffer(new char_type[bufsize]),
      _bufsize(bufsize)
  {
    setTimeout(timeout);
  }

  GnuTls_streambuf::int_type GnuTls_streambuf::overflow(GnuTls_streambuf::int_type c)
  {
    try
    {
      if (pptr() != pbase())
      {
        int n = _stream.sslWrite(pbase(), pptr() - pbase());
        if (n <= 0)
          return traits_type::eof();
      }

      setp(_buffer, _buffer + _bufsize);
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
    int n = _stream.sslRead(_buffer, _bufsize);
    if (n <= 0)
      return traits_type::eof();

    setg(_buffer, _buffer, _buffer + n);
    return (int_type)(unsigned char)_buffer[0];
  }

  int GnuTls_streambuf::sync()
  {
    try
    {
      if (pptr() != pbase())
      {
        int n = _stream.sslWrite(pbase(), pptr() - pbase());
        if (n <= 0)
          return -1;
        else
          setp(_buffer, _buffer + _bufsize);
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

