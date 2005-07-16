/* tnt/ssl.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef TNT_SSL_H
#define TNT_SSL_H

#include <cxxtools/tcpstream.h>
#include <openssl/ssl.h>

namespace tnt
{
  class SslException : public std::runtime_error
  {
      unsigned long code;

    public:
      SslException(const std::string& what, unsigned long code_)
        : std::runtime_error(what),
          code(code_)
      { }

      unsigned long getCode() const
      { return code; }
  };

  class SslServer : public cxxtools::net::Server
  {
      SSL_CTX* ctx;
      void installCertificates(const char* certificateFile, const char* privateKeyFile);

    public:
      SslServer(const char* certificateFile);
      SslServer(const char* certificateFile, const char* privateKeyFile);
      ~SslServer();

      SSL_CTX* getSslContext() const  { return ctx; }
  };

  class SslStream : public cxxtools::net::Stream
  {
      SSL* ssl;

    public:
      SslStream();

      explicit SslStream(int fd)
        : cxxtools::net::Stream(fd)
        { }

      explicit SslStream(const SslServer& server);
      ~SslStream();

      void Accept(const SslServer& server);

#if 0
currently no client-support
      void Connect(const char* ipaddr, unsigned short int port);
      void Connect(const std::string& ipaddr, unsigned short int port)
        { Connect(ipaddr.c_str(), port); }
#endif

      int SslRead(char* buffer, int bufsize) const;
      int SslWrite(const char* buffer, int bufsize) const;
  };

  class ssl_streambuf : public std::streambuf
  {
      SslStream& m_stream;
      char_type* m_buffer;
      unsigned m_bufsize;

    public:
      explicit ssl_streambuf(SslStream& stream, unsigned bufsize = 256, int timeout = -1);
      ~ssl_streambuf()
      { delete[] m_buffer; }

      void setTimeout(int t)   { m_stream.setTimeout(t); }
      int getTimeout() const   { return m_stream.getTimeout(); }

      /// überladen aus std::streambuf
      int_type overflow(int_type c);
      /// überladen aus std::streambuf
      int_type underflow();
      /// überladen aus std::streambuf
      int sync();
  };

  class ssl_iostream : public SslStream, public std::iostream
  {
      ssl_streambuf m_buffer;

    public:
      explicit ssl_iostream(unsigned bufsize = 256, int timeout = -1)
        : SslStream(-1),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      explicit ssl_iostream(const SslServer& server, unsigned bufsize = 256, int timeout = -1)
        : SslStream(server),
          std::iostream(&m_buffer),
          m_buffer(*this, bufsize, timeout)
        { }

      void setTimeout(int timeout)  { m_buffer.setTimeout(timeout); }
      int getTimeout() const        { return m_buffer.getTimeout(); }
  };
}

#endif // TNT_SSL_H

