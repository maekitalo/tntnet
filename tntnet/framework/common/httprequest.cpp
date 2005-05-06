/* httprequest.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <sstream>
#include <cxxtools/log.h>
#include <cxxtools/thread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace tnt
{
  log_define("tntnet.http");

  ////////////////////////////////////////////////////////////////////////
  // httpRequest
  //
  unsigned httpRequest::serial_ = 0;

  httpRequest::httpRequest(const std::string& url)
  : ssl(false),
    lang_init(false)
  {
    std::istringstream s("GET " + url + " HTTP/1.1\r\n\r\n");
    parse(s);
  }

  void httpRequest::clear()
  {
    httpMessage::clear();
    pathinfo.clear();
    args.clear();
    qparam.clear();
    ct = contenttype();
    mp = multipart();
    lang_init = false;
  }

  void httpRequest::parse(std::istream& in)
  {
    parser p(*this);
    p.parse(in);
    if (!p.failed())
      doPostParse();
  }

  void httpRequest::doPostParse()
  {
    qparam.parse_url(getQueryString());
    if (getMethod() == "POST")
    {
      std::istringstream in(getHeader(Content_Type));
      in >> ct;

      if (in)
      {
        log_debug(Content_Type << ' ' << in.str());
        if (ct.isMultipart())
        {
          log_debug("multipart-boundary=" << ct.getBoundary());
          mp.set(ct.getBoundary(), getBody());
          for (multipart::const_iterator it = mp.begin();
               it != mp.end(); ++it)
          {
            // hochgeladene Dateien nicht in qparam übernehmen
            if (it->getFilename().empty())
            {
              std::string multipartBody(it->getBodyBegin(), it->getBodyEnd());
              log_debug("multipart-item name=" << it->getName()
                     << " body=" << multipartBody);
              qparam.add(it->getName(), multipartBody);
            }
          }
        }
        else
        {
          qparam.parse_url(getBody());
        }
      }
      else
        qparam.parse_url(getBody());
    }

    {
      static cxxtools::Mutex monitor;
      cxxtools::MutexLock lock(monitor);
      serial = ++serial_;
    }
  }

  std::string httpRequest::getPeerIp() const
  {
    static cxxtools::Mutex monitor;
    cxxtools::MutexLock lock(monitor);

    char* p = inet_ntoa(peerAddr.sin_addr);
    return std::string(p);
  }

  std::string httpRequest::getServerIp() const
  {
    static cxxtools::Mutex monitor;
    cxxtools::MutexLock lock(monitor);

    char* p = inet_ntoa(serverAddr.sin_addr);
    return std::string(p);
  }

  std::string httpRequest::getLang() const
  {
    if (!lang_init)
    {
      static const std::string LANG = "LANG";
      log_debug("httpRequest::getLang() " << qparam.dump());

      lang = qparam[LANG];

      if (lang.empty())
      {
        const char* LANG = ::getenv("LANG");
        if (LANG)
          lang = LANG;
      }
      else
      {
        log_debug("LANG from query-parameter");
      }

      log_debug("LANG=" << lang);
      lang_init = true;
    }

    return lang;
  }

  const cookies& httpRequest::getCookies() const
  {
    log_debug("httpRequest::getCookies()");

    if (!httpcookies.hasCookies())
    {
      log_debug("cookies found");

      header_type::const_iterator it = header.find(Cookie);
      if (it != header.end())
      {
        log_debug("parse cookie-header " << it->second);
        const_cast<httpRequest*>(this)->httpcookies.set(it->second);
      }
    }

    return httpcookies;
  }

  bool httpRequest::keepAlive() const
  {
    header_type::const_iterator it = header.find(Connection);
    return it != header.end() && it->second == Connection_Keep_Alive;
  }

}
