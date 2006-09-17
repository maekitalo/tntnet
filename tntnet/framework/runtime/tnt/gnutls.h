/* tnt/gnutls.h
 * Copyright (C) 2006 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef TNT_GNUTLS_H
#define TNT_GNUTLS_H

#include <cxxtools/tcpstream.h>
#include <gnutls/gnutls.h>

namespace tnt
{
  class GnuTlsException : public std::runtime_error
  {
      unsigned long code;
      static std::string formatMessage(const std::string& function, int code_);

    public:
      GnuTlsException(const std::string& function, int code_)
        : std::runtime_error(formatMessage(function, code_)),
          code(code_)
      { }

      unsigned long getCode() const
      { return code; }
  };

  class GnuTlsInit
  {
      static unsigned initCount;
      static gnutls_dh_params dhParams;

    public:
      GnuTlsInit();
      ~GnuTlsInit();

      gnutls_dh_params getDhParams() const  { return dhParams; }
  };

  class GnuTlsX509Cred
  {
      gnutls_certificate_credentials x509_cred;
      GnuTlsInit init;

    public:
      GnuTlsX509Cred(const char* certificateFile, const char* privateKeyFile);
      ~GnuTlsX509Cred();

      operator gnutls_certificate_credentials() const { return x509_cred; }
  };

  class GnuTlsServer : public cxxtools::net::Server
  {
      GnuTlsX509Cred cred;

    public:
      GnuTlsServer(const char* certificateFile, const char* privateKeyFile);

      gnutls_certificate_credentials getCred() const  { return cred; }
  };

  class GnuTlsStream : public cxxtools::net::Stream
  {
      gnutls_session session;

    public:
      GnuTlsStream()
        : session(0)
        { }

      explicit GnuTlsStream(int fd)
        : cxxtools::net::Stream(fd)
        { }

      explicit GnuTlsStream(const GnuTlsServer& server)
        { accept(server); }

      ~GnuTlsStream();

      void accept(const GnuTlsServer& server);
      int sslRead(char* buffer, int bufsize) const;
      int sslWrite(const char* buffer, int bufsize) const;
      void shutdown() const;
  };

  class GnuTls_streambuf : public std::streambuf
  {
      GnuTlsStream& m_stream;
      char_type* m_buffer;
      unsigned m_bufsize;

    public:
      explicit GnuTls_streambuf(GnuTlsStream& stream, unsigned bufsize = 256, int timeout = -1);
      ~GnuTls_streambuf()
      { delete[] m_buffer; }

      void setTimeout(int t)   { m_stream.setTimeout(t); }
      int getTimeout() const   { return m_stream.getTimeout(); }

      /// overload std::streambuf
      int_type overflow(int_type c);
      /// overload std::streambuf
      int_type underflow();
      /// overload std::streambuf
      int sync();
  };

  class GnuTls_iostream : public GnuTlsStream, public std::iostream
  {
      GnuTls_streambuf m_buffer;

    public:
      explicit GnuTls_iostream(unsigned bufsize = 256, int timeout = -1)
        : GnuTlsStream(-1),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      explicit GnuTls_iostream(const GnuTlsServer& server, unsigned bufsize = 256, int timeout = -1)
        : GnuTlsStream(server),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      int getTimeout() const        { return m_buffer.getTimeout(); }
  };
}

#endif // TNT_GNUTLS_H

