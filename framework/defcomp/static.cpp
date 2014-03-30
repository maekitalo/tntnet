/*
 * Copyright (C) 2003,2007 Tommi Maekitalo
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

#include "static.h"
#include "mimehandler.h"
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httperror.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/comploader.h>
#include <tnt/tntconfig.h>
#include <fstream>
#include <cxxtools/log.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/convert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <config.h>
#include <limits>

#if defined(HAVE_SENDFILE) && defined(HAVE_SYS_SENDFILE_H)
#include <fcntl.h>
#include <sys/sendfile.h>
#include <cxxtools/net/tcpstream.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <unistd.h>
#include <typeinfo>
#include <errno.h>
#endif

log_define("tntnet.static")

namespace tnt
{
  namespace
  {
    bool parseRange(const char* range, off_t& offset, off_t& count)
    {
      enum
      {
        state_0,
        state_1,
        state_from0,
        state_from,
        state_from_e,
        state_to0,
        state_to,
        state_e
      } state = state_0;

      const char* prefix = "bytes=";
      off_t firstPos = 0;
      off_t lastPos = count;
      for (const char* p = range; *p && state != state_e; ++p)
      {
        char ch = *p;
        switch (state)
        {
          case state_0:
          case state_1:
            if (ch == *prefix)
            {
              ++prefix;
              if (*prefix == '\0')
              {
                log_debug("prefix ends");
                state = state_from0;
              }
              else
                state = state_1;
            }
            else if (state == state_1 || ch != ' ')
              return false;
            break;

          case state_from0:
            if (std::isdigit(ch))
            {
              log_debug("from found");
              firstPos = ch - '0';
              state = state_from;
            }
            else if (ch == '-')
              state = state_to0;
            else if (ch != ' ')
              return false;
            break;

          case state_from:
            if (std::isdigit(ch))
              firstPos = firstPos * 10 + ch - '0';
            else if (ch == '-')
              state = state_to0;
            else if (ch == ' ')
              state = state_from_e;
            else
              return false;
            break;

          case state_from_e:
            if (ch == '-')
              state = state_to0;
            else if (ch != ' ')
              return false;
            break;

          case state_to0:
            if (std::isdigit(ch))
            {
              lastPos = ch - '0';
              state = state_to;
            }
            else if (ch != ' ')
              return false;
            break;

          case state_to:
            if (std::isdigit(ch))
              lastPos = lastPos * 10 + ch - '0';
            else if (ch == ' ')
              state = state_e;
            else
              return false;
            break;

          case state_e: // not reachable but to satisfy the compiler we put it here
            break;
        }
      }

      switch (state)
      {
        case state_to0:
        case state_to:
        case state_e:
          if (lastPos > firstPos)
          {
            offset = firstPos;
            count = lastPos - firstPos;
            log_debug("firstPos=" << firstPos << " lastPos=" << lastPos << " offset=" << offset << " count=" << count);
            return true;
          }
          break;

        default:
          break;
      }

      return false;
    }

#if defined(HAVE_SENDFILE) && defined(HAVE_SYS_SENDFILE_H)
    class Fdfile
    {
      private:
        int _fd;
      public:
        Fdfile(const char* pathname, int flags)
          : _fd(open(pathname, flags))
        {
          if (_fd < 0)
            throw cxxtools::SystemError("open");
        }

        ~Fdfile()
        {
          if (_fd >= 0)
            ::close(_fd);
        }

        int getFd() const { return _fd; }
    };

    void pollout(int fd, int timeout)
    {
      struct pollfd fds;
      fds.fd = fd;
      fds.events = POLLOUT;

      log_debug("poll timeout " << timeout);

      int p = ::poll(&fds, 1, timeout);

      log_debug("poll returns " << p << " revents " << fds.revents);

      if (p < 0)
      {
        log_error("error in poll; errno=" << errno);
        throw cxxtools::SystemError("poll");
      }
      else if (p == 0)
      {
        log_debug("poll timeout (" << timeout << ')');
        throw cxxtools::IOTimeout();
      }
    }

#endif
  }

  Static::~Static()
    { delete _handler; }

  void Static::configure(const TntConfig& config)
  {
    if (_handler == 0)
      _handler = new MimeHandler();
  }

  static ComponentFactoryImpl<Static> staticFactory("static");

  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  void Static::setContentType(HttpRequest& request, HttpReply& reply)
  {
    if (_handler)
      reply.setContentType(_handler->getMimeType(request.getPathInfo()).c_str());
  }

  unsigned Static::operator() (HttpRequest& request, HttpReply& reply, QueryParams& qparams)
    { return doCall(request, reply, qparams, false); }

  unsigned Static::topCall(HttpRequest& request, HttpReply& reply, QueryParams& qparams)
    { return doCall(request, reply, qparams, true); }

  unsigned Static::doCall(HttpRequest& request, HttpReply& reply, QueryParams& qparams, bool top)
  {
    if (!tnt::HttpRequest::checkUrl(request.getPathInfo())
      || request.getPathInfo().find('\0') != std::string::npos)
      throw tnt::HttpError(HTTP_BAD_REQUEST, "illegal url");

    // fetch document root from arguments or take global setting as default

    std::string file = request.getArg("documentRoot", TntConfig::it().documentRoot);
    log_debug("document root =\"" << file << '"');

    if (!file.empty() && *file.rbegin() != '/')
      file += '/';

    file += request.getPathInfo();

    log_debug("file: " << file);

    struct stat st;

    bool localEnableGzip = false;
    if (request.getEncoding().accept("gzip") && TntConfig::it().enableCompression)
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
        log_debug("can't stat file \"" << file << "\"");
        return DECLINED;
      }

      if (!S_ISREG(st.st_mode))
      {
        log_debug("no regular file \"" << file << "\"");
        return DECLINED;
      }
    }

    off_t offset = 0;
    off_t count = st.st_size;
    unsigned httpOkReturn = HTTP_OK;

    if (top)
    {
      // set Content-Type
      std::string contentType = request.getArg("contentType");
      if (!contentType.empty())
      {
        log_debug("content type is \"" << contentType << '"');
        reply.setContentType(contentType.c_str());
      }
      else
        setContentType(request, reply);

      std::string lastModified = HttpMessage::htdate(st.st_ctime);

      {
        std::string s = request.getHeader(httpheader::ifModifiedSince);
        if (s == lastModified)
          return HTTP_NOT_MODIFIED;
      }

      reply.setHeader(httpheader::lastModified, lastModified);
      reply.setKeepAliveHeader();
      reply.setHeader(httpheader::acceptRanges, "bytes");

      if (!reply.hasHeader(httpheader::cacheControl))
      {
        std::string maxAgeStr = request.getArg("maxAge");
        unsigned maxAge = maxAgeStr.empty() ? 14400 : cxxtools::convert<unsigned>(maxAgeStr);
        reply.setMaxAgeHeader(maxAge);
      }

      // check for byte range (only "bytes=from-" or "bytes=from-to" are supported)
      const char* range = request.getHeader(httpheader::range, 0);
      if (range)
      {
        if (parseRange(range, offset, count))
        {
          if (offset > st.st_size)
            return HTTP_RANGE_NOT_SATISFIABLE;

          reply.setHeader(httpheader::contentLocation, request.getUrl());
          std::ostringstream contentRange;
          contentRange << offset << '-' << (offset+count)-1 << '/' << st.st_size;
          reply.setHeader(httpheader::contentRange, contentRange.str());

          httpOkReturn = HTTP_PARTIAL_CONTENT;
        }
        else
          log_debug("ignore invalid byte range " << range);
      }

      // set Content-Length
      reply.setContentLengthHeader(count);

      if (request.isMethodHEAD())
      {
        log_debug("head request");
        return httpOkReturn;
      }
      else
      {
        log_debug("no head request");
      }

      // send data
      log_info("send file \"" << file << "\" size " << st.st_size << " bytes; offset=" << offset << " count=" << count);

#if defined(HAVE_SENDFILE) && defined(HAVE_SYS_SENDFILE_H)
      int on = 1;
      int off = 0;
      try
      {
        cxxtools::net::iostream& tcpStream = dynamic_cast<cxxtools::net::iostream&>(reply.getDirectStream());

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_NODELAY,
            &off, sizeof(off)) < 0)
          throw cxxtools::SystemError("setsockopt(TCP_NODELAY)");

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_CORK,
            &on, sizeof(on)) < 0)
          throw cxxtools::SystemError("setsockopt(TCP_CORK)");

        reply.setDirectMode(httpOkReturn, HttpReturn::httpMessage(httpOkReturn));
        tcpStream.flush();

        Fdfile in(file.c_str(), O_RDONLY);
        ssize_t s;
        while(tcpStream)
        {
          do
          {
            log_debug("sendfile offset " << offset << " size " << count);
            s = sendfile(tcpStream.getFd(), in.getFd(), &offset, count);
            log_debug("sendfile returns " << s);
          } while (s < 0 && errno == EINTR);

          if (s < 0 && errno != EAGAIN)
            throw cxxtools::SystemError("sendfile");

          if (offset >= count)
            break;

          log_debug("poll");
          pollout(tcpStream.getFd(), tcpStream.getTimeout());
        }

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_CORK,
            &off, sizeof(off)) < 0)
          throw cxxtools::SystemError("setsockopt(TCP_CORK)");

        if (::setsockopt(tcpStream.getFd(), SOL_TCP, TCP_NODELAY,
            &on, sizeof(on)) < 0)
          throw cxxtools::SystemError("setsockopt(TCP_NODELAY)");

        return httpOkReturn;
      }
      catch (const std::bad_cast& e)
      {
        log_debug("stream is no tcpstream - don't use sendfile");
        reply.setDirectMode();
      }
#else
      reply.setDirectMode();
#endif
    }

    std::ifstream in(file.c_str());
    in.seekg(offset);
    if (!in)
    {
      log_debug("can't open file \"" << file << '"');
      return DECLINED;
    }

    if (offset == 0 && count == st.st_size)
    {
      reply.out() << in.rdbuf() << std::flush;
      if (in.fail())
        throw std::runtime_error("failed to send file \"" + file + '"');
    }
    else
    {
      char ch;
      off_t o;
      for (o = 0; in.get(ch) && o < count; ++o)
        reply.out().put(ch);
      if (o < count)
        throw std::runtime_error("failed to send file \"" + file + '"');
    }

    return httpOkReturn;
  }
}

