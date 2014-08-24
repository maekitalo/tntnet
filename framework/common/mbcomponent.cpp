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
#include <cxxtools/convert.h>
#include <cstring>
#include <climits>

log_define("tntnet.mbcomponent")

namespace tnt
{
  namespace
  {
    inline bool charpLess(const char* a, const char* b)
      { return std::strcmp(a, b) < 0; }
  }

  void MbComponent::init(const char* rawData, const char** urls,
                         const char** mimetypes, const char** ctimes)
  {
    _rawData   = rawData;
    _urls      = urls;
    _mimetypes = mimetypes;
    _ctimes    = ctimes;

    tnt::DataChunks data(rawData);
    _compressedData.resize(data.size());
  }

  unsigned MbComponent::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
    { return doCall(request, reply, qparam, false); }

  unsigned MbComponent::topCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
    { return doCall(request, reply, qparam, true); }

  unsigned MbComponent::searchFile(const std::string& url, const tnt::DataChunks& data) const
  {
    log_debug("search for \"" << url << '"');

    const char** urls_end = _urls + data.size();
    const char** it = std::lower_bound(_urls, urls_end, url.c_str(), charpLess);

    if (it == urls_end || std::strcmp(url.c_str(), *it) != 0)
    {
      log_debug("file \"" << url << "\" not found");
      return UINT_MAX;
    }

    const unsigned idx = it - _urls;
    log_debug("file \"" << url << "\" found; idx=" << idx);
    return idx;
  }

  unsigned MbComponent::doCall(tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams&, bool top)
  {
    log_trace("MbComponent " << getCompident());

    tnt::DataChunks data(_rawData);

    unsigned url_idx = 0;

    const std::string url(request.getPathInfo());

    if (_urls)
    {
      url_idx = searchFile(url, data);
      if(url_idx == UINT_MAX) {
        //file was not found
        return DECLINED;
      }
    }

    if (top)
    {
      reply.setKeepAliveHeader();
      reply.setContentType(_mimetypes[url_idx]);

      if (!reply.hasHeader(httpheader::cacheControl))
      {
        std::string maxAgeStr = request.getArg("maxAge");
        unsigned maxAge = maxAgeStr.empty() ? 14400 : cxxtools::convert<unsigned>(maxAgeStr);
        reply.setMaxAgeHeader(maxAge);
      }

      std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);
      if (s == _ctimes[url_idx])
        return HTTP_NOT_MODIFIED;

      reply.setHeader(tnt::httpheader::lastModified, _ctimes[url_idx]);

      if (request.getEncoding().accept("gzip") && TntConfig::it().enableCompression)
      {

        //search for a file ending with .gz, containing the requested content already compressed
        if(url.substr(url.size()-3, 3) != ".gz") 
        {
          unsigned comp_url_idx = searchFile(url + ".gz", data);
          if (comp_url_idx != UINT_MAX)
          {
            //file was found
            log_debug("size canged from " << data[url_idx].getLength() << " to " << data[comp_url_idx].getLength());
            reply.setContentLengthHeader(data[comp_url_idx].getLength());
            reply.setHeader(httpheader::contentEncoding, "gzip");
            if (!request.isMethodHEAD())
            {
              reply.setDirectMode();
              reply.out() << data[comp_url_idx];
            }
            return HTTP_OK;
          }
        }

        cxxtools::ReadLock lock(_mutex);
        if (_compressedData[url_idx].empty())
        {
          lock.unlock();
          cxxtools::WriteLock wlock(_mutex);

          if (_compressedData[url_idx].empty())
          {
            // check for compression
            std::string body(data[url_idx].getData(), data[url_idx].getLength());
            log_info("try compress");
            if (reply.tryCompress(body))
            {
              log_info("compressed successfully from " << data[url_idx].getLength() << " to " << body.size());
              _compressedData[url_idx] = body;
            }
            else
            {
              log_info("not compressed " << data[url_idx].getLength());
              _compressedData[url_idx] = "-";
            }
          }
        }

        if (_compressedData[url_idx] != "-")
        {
          log_debug("compressed data found; content size " << data[url_idx].getLength() << " to " << _compressedData[url_idx].size());
          reply.setContentLengthHeader(_compressedData[url_idx].size());
          reply.setHeader(httpheader::contentEncoding, "gzip");
          if (!request.isMethodHEAD())
          {
            reply.setDirectMode();
            reply.out() << _compressedData[url_idx];
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

