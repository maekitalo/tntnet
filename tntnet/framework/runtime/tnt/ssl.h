/* tnt/ssl.h
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

      void accept(const SslServer& server);

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

