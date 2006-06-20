/* tnt/openssl.h
 * Copyright (C) 2003 Tommi Maekitalo
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

#ifndef TNT_OPENSSL_H
#define TNT_OPENSSL_H

#include <cxxtools/tcpstream.h>
#include <openssl/ssl.h>

namespace tnt
{
  class OpensslException : public std::runtime_error
  {
      unsigned long code;

    public:
      OpensslException(const std::string& what, unsigned long code_)
        : std::runtime_error(what),
          code(code_)
      { }

      unsigned long getCode() const
      { return code; }
  };

  class OpensslServer : public cxxtools::net::Server
  {
      SSL_CTX* ctx;
      void installCertificates(const char* certificateFile, const char* privateKeyFile);

    public:
      OpensslServer(const char* certificateFile);
      OpensslServer(const char* certificateFile, const char* privateKeyFile);
      ~OpensslServer();

      SSL_CTX* getSslContext() const  { return ctx; }
  };

  class OpensslStream : public cxxtools::net::Stream
  {
      SSL* ssl;

    public:
      OpensslStream();

      explicit OpensslStream(int fd)
        : cxxtools::net::Stream(fd)
        { }

      explicit OpensslStream(const OpensslServer& server);
      ~OpensslStream();

      void accept(const OpensslServer& server);

      int sslRead(char* buffer, int bufsize) const;
      int sslWrite(const char* buffer, int bufsize) const;
      void shutdown() const;
  };

  class openssl_streambuf : public std::streambuf
  {
      OpensslStream& m_stream;
      char_type* m_buffer;
      unsigned m_bufsize;

    public:
      explicit openssl_streambuf(OpensslStream& stream, unsigned bufsize = 256, int timeout = -1);
      ~openssl_streambuf()
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

  class openssl_iostream : public OpensslStream, public std::iostream
  {
      openssl_streambuf m_buffer;

    public:
      explicit openssl_iostream(unsigned bufsize = 256, int timeout = -1)
        : OpensslStream(-1),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      explicit openssl_iostream(const OpensslServer& server, unsigned bufsize = 256, int timeout = -1)
        : OpensslStream(server),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      int getTimeout() const        { return m_buffer.getTimeout(); }
  };
}

#endif // TNT_OPENSSL_H

