////////////////////////////////////////////////////////////////////////
// ssl.cpp
//

#include "tnt/ssl.h"
#include <cxxtools/thread.h>
#include <openssl/err.h>
#include "tnt/log.h"

namespace tnt
{
  static const char * SSLerrmessage(void);

  static bool initialized = false;
  static Mutex mutex;
  static SSL_CTX* ctx;

  static void checkSslError()
  {
    unsigned long code = ERR_get_error();
    if (code != 0)
    {
      char buffer[120];
      if (ERR_error_string(code, buffer))
        throw SslException(buffer, code);
      else
        throw SslException("unknown SSL-Error", code);
    }
  }

  static void ssl_init()
  {
    if (!initialized)
    {
      MutexLock lock(mutex);
      if (!initialized)
      {
        SSL_load_error_strings();
        SSL_library_init();
        checkSslError();
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

    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, certificateFile);
  }

  SslServer::SslServer(const char* certificateFile, const char* privateKeyFile)
  {
    ssl_init();

    ctx = SSL_CTX_new(SSLv23_server_method());
    checkSslError();

    installCertificates(certificateFile, privateKeyFile);
  }

  SslServer::~SslServer()
  {
    if (ctx)
      SSL_CTX_free(ctx);
  }

  //////////////////////////////////////////////////////////////////////
  // SslStream
  //
  SslStream::SslStream()
    : ssl(0)
  {
  }

  SslStream::SslStream(const SslServer& server)
    : ssl( SSL_new( server.getSslContext() ) )
  {
    checkSslError();

    SSL_set_fd(ssl, getFd());

    SSL_accept(ssl);
    checkSslError();
  }

  SslStream::~SslStream()
  {
    if (ssl)
      SSL_free(ssl);
  }

  void SslStream::Accept(const SslServer& server)
  {
    Stream::Accept(server);

    log_debug("tcp-connection established - build ssltunnel");
    ssl = SSL_new( server.getSslContext() );
    checkSslError();

    SSL_set_fd(ssl, getFd());

    SSL_accept(ssl);
    checkSslError();

    log_debug("ssl-connection ready");
  }

  int SslStream::SslRead(char* buffer, int bufsize, int timeout) const
  {
    int count = SSL_read(ssl, buffer, bufsize);
    checkSslError();
    return count;
  }

  int SslStream::SslWrite(const char* buffer, int bufsize) const
  {
    int count = SSL_write(ssl, buffer, bufsize);
    checkSslError();
    return count;
  }

  //////////////////////////////////////////////////////////////////////
  // ssl_streambuf
  //
  ssl_streambuf::ssl_streambuf(SslStream& stream, unsigned bufsize, int timeout)
    : m_stream(stream),
      m_bufsize(bufsize),
      m_buffer(new char_type[bufsize]),
      m_timeout(timeout)
  { }

  ssl_streambuf::int_type ssl_streambuf::overflow(ssl_streambuf::int_type c)
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

  ssl_streambuf::int_type ssl_streambuf::underflow()
  {
    int n = m_stream.SslRead(m_buffer, m_bufsize, m_timeout);
    if (n <= 0)
      return traits_type::eof();

    setg(m_buffer, m_buffer, m_buffer + n);
    return (int_type)(unsigned char)m_buffer[0];
  }

  int ssl_streambuf::sync()
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

  //////////////////////////////////////////////////////////////////////
  // ssl_iostream
  //
}
