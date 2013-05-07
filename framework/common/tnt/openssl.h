/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#ifndef TNT_OPENSSL_H
#define TNT_OPENSSL_H

#include <cxxtools/net/tcpstream.h>
#include <cxxtools/smartptr.h>
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

  // destroy policy for smart pointer
  template <typename ctx>
  class SslCtxReleaser;

  template <>
  class SslCtxReleaser<SSL_CTX>
  {
    protected:
      void destroy(SSL_CTX* ctx);
  };

  typedef cxxtools::SmartPtr<SSL_CTX, cxxtools::ExternalRefCounted, SslCtxReleaser> SslCtxPtr;

  class OpensslServer : public cxxtools::net::TcpServer
  {
    public:

    private:
      SslCtxPtr ctx;
      void installCertificates(const char* certificateFile, const char* privateKeyFile);

    public:
      explicit OpensslServer(const char* certificateFile);
      OpensslServer(const char* certificateFile, const char* privateKeyFile);

      SslCtxPtr getSslContext() const        { return ctx; }
  };

  class OpensslStream : public cxxtools::net::TcpSocket
  {
      SslCtxPtr ctx;
      SSL* ssl;

    public:
      OpensslStream();

      explicit OpensslStream(const OpensslServer& server, bool inherit = false);
      ~OpensslStream();

      void accept(const OpensslServer& server, bool inherit = false);
      void handshake(const OpensslServer& server);

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
      explicit openssl_streambuf(OpensslStream& stream, unsigned bufsize = 8192, int timeout = -1);
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
      explicit openssl_iostream(unsigned bufsize = 8192, int timeout = -1)
        : std::iostream(0),
          m_buffer(*this, bufsize, timeout)
        { init(&m_buffer); }

      explicit openssl_iostream(const OpensslServer& server, unsigned bufsize = 8192, int timeout = -1)
        : OpensslStream(server),
          std::iostream(0),
          m_buffer(*this, bufsize, timeout)
        { init(&m_buffer); }

      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      int getTimeout() const        { return m_buffer.getTimeout(); }
  };
}

#endif // TNT_OPENSSL_H

