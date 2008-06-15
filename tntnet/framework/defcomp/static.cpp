/* static.cpp
 * Copyright (C) 2003,2007 Tommi Maekitalo
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

#include "static.h"
#include "mimehandler.h"
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httperror.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/httperror.h>
#include <tnt/comploader.h>
#include <fstream>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <config.h>

#if HAVE_SENDFILE
#include <fcntl.h>
#include <sys/sendfile.h>
#include <cxxtools/syserror.h>
#include <cxxtools/tcpstream.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <typeinfo>
#include <errno.h>
#endif

log_define("tntnet.static")

namespace tnt
{
#if HAVE_SENDFILE
  namespace
  {
    class Fdfile
    {
        int fd;
      public:
        Fdfile(const char* pathname, int flags)
          : fd(open(pathname, flags))
        {
          if (fd < 0)
            throw cxxtools::SysError("open");
        }

        ~Fdfile()
        {
          if (fd >= 0)
            ::close(fd);
        }

        int getFd() const  { return fd; }
    };
  }
#endif

  tnt::Component* StaticFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Static();
  }

  void StaticFactory::doConfigure(const tnt::Tntconfig& config)
  {
    if (Static::handler == 0)
      Static::handler = new MimeHandler(config);

    Static::documentRoot = config.getValue(Static::configDocumentRoot);
    Static::enableGzip = config.getBoolValue("StaticEnableGzip", Static::enableGzip);
  }

  static StaticFactory staticFactory("static");

  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  std::string Static::configDocumentRoot = "DocumentRoot";
  std::string Static::documentRoot;
  MimeHandler* Static::handler;
  bool Static::enableGzip = true;

  void Static::setContentType(tnt::HttpRequest& request, tnt::HttpReply& reply)
  {
    if (handler)
      reply.setContentType(handler->getMimeType(request.getPathInfo()));
  }

  unsigned Static::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, tnt::QueryParams& qparams)
  {
    return operator() (request, reply, qparams, false);
  }

  unsigned Static::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, tnt::QueryParams& qparams, bool top)
  {
    if (!tnt::HttpRequest::checkUrl(request.getPathInfo()))
      throw tnt::HttpError(HTTP_BAD_REQUEST, "illegal url");

    std::string file = request.getArg(configDocumentRoot); // fetch document root from arguments first
    if (file.empty())
    {
      if (!documentRoot.empty())
        file = documentRoot;
    }
    else
      log_debug("document root \"" << file << "\" got as argument");

    if (!file.empty() && *file.rbegin() != '/')
      file += '/';

    file += request.getPathInfo();

    log_debug("file: " << file);

    struct stat st;

    bool localEnableGzip = false;
    if (request.getEncoding().accept("gzip") && enableGzip)
    {
      std::string gzfile = file + ".gz";
      if (stat(gzfile.c_str(), &st) == 0 && S_ISREG(st.st_mode))
      {
        log_debug("enable compression");
        file = gzfile;
        localEnableGzip = true;
        reply.setHeader(httpheader::contentEncoding, "gzip");
      }
      else
      {
        log_debug("compressed file \"" << gzfile << "\" not found or not a regular file");
      }
    }

    if (!localEnableGzip)
    {
      if (stat(file.c_str(), &st) != 0)
      {
        log_warn("error in stat for file \"" << file << "\"");
        return DECLINED;
      }

      if (!S_ISREG(st.st_mode))
      {
        log_warn("no regular file \"" << file << "\"");
        return DECLINED;
      }
    }

    std::string lastModified = tnt::HttpMessage::htdate(st.st_ctime);

    {
      std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);
      if (s == lastModified)
        return HTTP_NOT_MODIFIED;
    }

    // set Content-Type
    std::string contentType = request.getArg("ContentType");
    if (!contentType.empty())
    {
      log_debug("content type is \"" << contentType << '"');
      reply.setContentType(contentType);
    }
    else
      setContentType(request, reply);

    reply.setHeader(tnt::httpheader::lastModified, lastModified);

    // set Keep-Alive
    reply.setKeepAliveHeader();

    // set Content-Length
    reply.setContentLengthHeader(st.st_size);

    // send data
    log_info("send file \"" << file << "\" size " << st.st_size << " bytes");

#if HAVE_SENDFILE
    if (top)
    {
      int on = 1;
      int off = 0;
      try
      {
        cxxtools::net::iostream& tcpStream = dynamic_cast<cxxtools::net::iostream&>(reply.getDirectStream());

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_NODELAY,
            &off, sizeof(off)) < 0)
          throw cxxtools::net::Exception("setsockopt(TCP_NODELAY)");

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_CORK,
            &on, sizeof(on)) < 0)
          throw cxxtools::net::Exception("setsockopt(TCP_CORK)");

        reply.setDirectMode();
        tcpStream.flush();

        off_t offset = 0;
        Fdfile in(file.c_str(), O_RDONLY);
        ssize_t s;
        while(tcpStream)
        {
          do
          {
            log_debug("sendfile " << st.st_size << " bytes");
            s = sendfile(tcpStream.getFd(), in.getFd(), &offset, st.st_size - offset);
            log_debug("sendfile returns " << s);
          } while (s < 0 && errno == EINTR);

          if (s < 0 && errno != EAGAIN)
            throw cxxtools::SysError("sendfile");

          if (offset >= st.st_size)
            break;

          log_debug("poll");
          tcpStream.poll(POLLOUT);
        }

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_CORK,
            &off, sizeof(off)) < 0)
          throw cxxtools::net::Exception("setsockopt(TCP_CORK)");

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_NODELAY,
            &on, sizeof(on)) < 0)
          throw cxxtools::net::Exception("setsockopt(TCP_NODELAY)");

        return HTTP_OK;
      }
      catch (const std::bad_cast& e)
      {
        log_debug("stream is no tcpstream - don't use sendfile");
        reply.setDirectMode();
      }
    }
#else
    if (top)
      reply.setDirectMode();
#endif
    std::ifstream in(file.c_str());
    if (!in)
    {
      log_warn("error opening file \"" << file << '"');
      return DECLINED;
    }

    reply.out() << in.rdbuf() << std::flush;

    return HTTP_OK;
  }

}
