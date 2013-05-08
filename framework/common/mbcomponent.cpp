/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <tnt/mbcomponent.h>
#include <tnt/data.h>
#include <tnt/http.h>
#include <tnt/httpreply.h>
#include <cxxtools/log.h>
#include <cstring>

log_define("tntnet.mbcomponent")

namespace tnt
{
  namespace
  {
    inline bool charpLess(const char* a, const char* b)
    {
      return std::strcmp(a, b) < 0;
    }
  }

  void MbComponent::init(const char* rawData_, const char** urls_,
                         const char** mimetypes_, const char** ctimes_)
  {
    rawData   = rawData_;
    urls      = urls_;
    mimetypes = mimetypes_;
    ctimes    = ctimes_;

    tnt::DataChunks data(rawData);
    compressedData.resize(data.size());
  }

  unsigned MbComponent::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
  {
    return doCall(request, reply, qparam, false);
  }

  unsigned MbComponent::topCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
  {
    return doCall(request, reply, qparam, true);
  }

  unsigned MbComponent::doCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam, bool top)
  {
    log_trace("MbComponent " << getCompident());

    tnt::DataChunks data(rawData);

    unsigned url_idx = 0;
    if (urls)
    {
      const char* url = request.getPathInfo().c_str();

      log_debug("search for \"" << url << '"');

      const char** urls_end = urls + data.size();
      const char** it = std::lower_bound(urls, urls_end, url, charpLess);
      if (it == urls_end || std::strcmp(url, *it) != 0)
      {
        log_debug("file \"" << url << "\" not found");
        return DECLINED;
      }

      url_idx = it - urls;

      log_debug("file \"" << url << "\" found; idx=" << url_idx);
    }

    if (top)
    {
      reply.setKeepAliveHeader();
      reply.setContentType(mimetypes[url_idx]);

      std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);
      if (s == ctimes[url_idx])
        return HTTP_NOT_MODIFIED;

      reply.setHeader(tnt::httpheader::lastModified, ctimes[url_idx]);

      if (request.getEncoding().accept("gzip"))
      {
        cxxtools::ReadLock lock(mutex);
        if (compressedData[url_idx].empty())
        {
          lock.unlock();
          cxxtools::WriteLock wlock(mutex);

          if (compressedData[url_idx].empty())
          {
            // check for compression
            std::string body(data[url_idx].getData(), data[url_idx].getLength());
            log_info("try compress");
            if (reply.tryCompress(body))
            {
              log_info("compressed successfully from " << data[url_idx].getLength() << " to " << body.size());
              compressedData[url_idx] = body;
            }
            else
            {
              log_info("not compressed " << data[url_idx].getLength());
              compressedData[url_idx] = "-";
            }
          }
        }

        if (compressedData[url_idx] != "-")
        {
          log_debug("compressed data found; content size " << data[url_idx].getLength() << " to " << compressedData[url_idx].size());
          reply.setContentLengthHeader(compressedData[url_idx].size());
          reply.setHeader(httpheader::contentEncoding, "gzip");
          if (!request.isMethodHEAD())
          {
            reply.setDirectMode();
            reply.out() << compressedData[url_idx];
          }
          return HTTP_OK;
        }
      }

      reply.setContentLengthHeader(data.size(url_idx));

      if (!request.isMethodHEAD())
      {
        log_debug("send data");
        reply.setDirectMode();
      }
    }

    if (!request.isMethodHEAD())
      reply.out() << data[url_idx];

    return HTTP_OK;
  }

}
