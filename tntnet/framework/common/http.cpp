/* http.cpp
   Copyright (C) 2003 Tommi MÃ¤kitalo

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

#include <tnt/http.h>
#include <tnt/contenttype.h>
#include <tnt/multipart.h>
#include <tnt/contentdisposition.h>

#include <list>
#include <algorithm>
#include <arpa/inet.h>
#include <cxxtools/thread.h>
#include <cxxtools/md5stream.h>
#include <cxxtools/log.h>

namespace tnt
{
  const std::string httpMessage::Content_Type = "Content-Type:";
  const std::string httpMessage::Content_Length = "Content-Length:";
  const std::string httpMessage::Connection = "Connection:";
  const std::string httpMessage::Connection_close = "close";
  const std::string httpMessage::Connection_Keep_Alive = "Keep-Alive";
  const std::string httpMessage::Last_Modified = "Last-Modified:";
  const std::string httpMessage::Server = "Server:";
  const std::string httpMessage::ServerName = "tntnet 1.1";
  const std::string httpMessage::Location = "Location:";
  const std::string httpMessage::AcceptLanguage = "Accept-Language:";
  const std::string httpMessage::Date = "Date:";
  const std::string httpMessage::KeepAlive = "Keep-Alive:";
  const std::string httpMessage::KeepAliveParam = "timeout=15, max=10";
  const std::string httpMessage::IfModifiedSince = "If-Modified-Since:";
  const std::string httpMessage::Host = "Host:";
  const std::string httpMessage::CacheControl = "Cache-Control:";
  const std::string httpMessage::Content_MD5 = "Content-MD5:";

////////////////////////////////////////////////////////////////////////
// httpMessage
//
log_define_static("tntnet.http");

void httpMessage::clear()
{
  method.clear();
  url.clear();
  query_string.clear();
  header.clear();
  body.clear();
  major_version = 0;
  minor_version = 0;
  content_size = 0;
}

void httpMessage::parse(std::istream& in)
{
  parseStartline(in);
  log_debug("method=" << getMethod());
  if (in)
  {
    header.parse(in);
    if (in)
      parseBody(in);
  }
}

std::string httpMessage::getHeader(const std::string& key, const std::string& def) const
{
  header_type::const_iterator i = header.find(key);
  return i == header.end() ? def : i->second;
}

void httpMessage::setHeader(const std::string& key, const std::string& value)
{
  log_debug("httpMessage::setHeader(\"" << key << "\", \"" << value << "\")");
  header.insert(header_type::value_type(key, value));
}

void httpMessage::setContentLengthHeader(size_t size)
{
  std::ostringstream s;
  s << size;
  setHeader(Content_Length, s.str());
}

std::string httpMessage::dumpHeader() const
{
  std::ostringstream h;
  dumpHeader(h);
  return h.str();
}

void httpMessage::dumpHeader(std::ostream& out) const
{
  for (header_type::const_iterator it = header.begin();
       it != header.end(); ++it)
    out << it->first << ' ' << it->second << '\n';
}

bool httpMessage::keepAlive() const
{
  header_type::const_iterator it = header.find(Connection);
  if (it == header.end())
    return false;

  if (getMajorVersion() == 1
   && getMinorVersion() == 1)
  {
    // keep-Alive if value not "close"
    return it->second != Connection_close;
  }
  else
    return it->second == Connection_Keep_Alive;
}

std::string httpMessage::htdate(time_t t)
{
  struct tm tm;
  localtime_r(&t, &tm);
  return htdate(&tm);
}

std::string httpMessage::htdate(struct tm* tm)
{
  const char* wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* monthn[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char buffer[80];

  mktime(tm);
  sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
    wday[tm->tm_wday], tm->tm_mday, monthn[tm->tm_mon], tm->tm_year + 1900,
    tm->tm_hour, tm->tm_min, tm->tm_sec);
  return buffer;
}

void httpMessage::parseStartline(std::istream& in)
{
  enum state_type
  {
    state_cmd0,
    state_cmd,
    state_url0,
    state_url,
    state_qparam,
    state_version,
    state_version_major,
    state_version_minor,
    state_end0,
    state_end
  };

  state_type state = state_cmd0;
  clear();

  std::streambuf* buf = in.rdbuf();

  while (state != state_end
      && buf->sgetc() != std::ios::traits_type::eof())
  {
    char ch = buf->sbumpc();
    switch(state)
    {
      case state_cmd0:
        if (!std::isspace(ch))
        {
          method = ch;
          state = state_cmd;
        }
        break;

      case state_cmd:
        if (std::isspace(ch))
          state = state_url0;
        else
          method += ch;
        break;

      case state_url0:
        if (!std::isspace(ch))
        {
          url = ch;
          state = state_url;
        }
        break;

      case state_url:
        if (ch == '?')
          state = state_qparam;
        else if (std::isspace(ch))
          state = state_version;
        else
          url += ch;
        break;

      case state_qparam:
        if (std::isspace(ch))
          state = state_version;
        else
          query_string += ch;
        break;

      case state_version:
        if (ch == '/')
          state = state_version_major;
        else if (ch == '\r')
          in.setstate(std::ios::failbit);
        break;

      case state_version_major:
        if (ch == '.')
          state = state_version_minor;
        else if (std::isdigit(ch))
          major_version = major_version * 10 + (ch - '0');
        else
          in.setstate(std::ios::failbit);
        break;

      case state_version_minor:
        if (ch == '\n')
          state = state_end;
        else if (std::isspace(ch))
          state = state_end0;
        else if (std::isdigit(ch))
          minor_version = minor_version * 10 + (ch - '0');
        else
          in.setstate(std::ios::failbit);
        break;

      case state_end0:
        if (ch == '\n')
          state = state_end;
        break;

      case state_end:
        break;
    }
  }

  if (url.compare(0, 7, "http://") == 0 || url.compare(0, 7, "HTTP://") == 0)
    url.erase(0, url.find('/', 7));
  else if (url.compare(0, 8, "https://") == 0 || url.compare(0, 8, "HTTPS://") == 0)
    url.erase(0, url.find('/', 8));

  if (state != state_end)
    in.setstate(std::ios_base::eofbit);

  if (!in)
    log_warn("error reading http-Message in state s" << state);
}

void httpMessage::parseBody(std::istream& in)
{
  std::string content_length_header = getHeader("Content-Length:");
  if (!content_length_header.empty())
  {
    std::istringstream valuestream(content_length_header);
    valuestream >> content_size;
    if (!valuestream)
      throw httpError("400 missing Content-Length");

    body.clear();
    char buffer[512];
    size_t size = content_size;
    while (size > 0
      && (in.read(buffer, std::min(sizeof(buffer), size)), in.gcount() > 0))
    {
      body.append(buffer, in.gcount());
      size -= in.gcount();
    }
  }
}

namespace
{
  class pstr
  {
      const char* start;
      const char* end;

    public:
      typedef size_t size_type;

      pstr(const char* s, const char* e)
        : start(s), end(e)
        { }

      size_type size() const  { return end - start; }

      bool operator== (const pstr& s) const
      {
        return size() == s.size()
          && std::equal(start, end, s.start);
      }

      bool operator== (const char* str) const
      {
        size_type i;
        for (i = 0; i < size() && str[i] != '\0'; ++i)
          if (start[i] != str[i])
            return false;
        return i == size() && str[i] == '\0';
      }
  };
}

bool httpMessage::checkUrl(const std::string& url)
{
  typedef std::list<pstr> tokens_type;

  // teile in Komponenten auf
  enum state_type {
    state_start,
    state_token,
    state_end
  };

  state_type state = state_start;

  tokens_type tokens;
  const char* p = url.data();
  const char* e = p + url.size();
  const char* s = p;
  for (; state != state_end && p != e; ++p)
  {
    switch (state)
    {
      case state_start:
        if (*p != '/')
        {
          s = p;
          state = state_token;
        }
        break;

      case state_token:
        if (*p == '/')
        {
          tokens.push_back(pstr(s, p));
          state = state_start;
        }
        else if (p == e)
        {
          tokens.push_back(pstr(s, p));
          state = state_end;
        }
        break;

      case state_end:
        break;
    }
  }

  // entferne jedes .. inklusive der vorhergehenden Komponente
  tokens_type::iterator i = tokens.begin();
  while (i != tokens.end())
  {
    if (*i == "..")
    {
      if (i == tokens.begin())
        return false;
      --i;
      i = tokens.erase(i);  // lösche 2 Komponenten
      i = tokens.erase(i);
    }
    else
    {
      ++i;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////
// httpRequest
//
unsigned httpRequest::serial_ = 0;

httpRequest::httpRequest(const std::string& url)
: ssl(false),
  lang_init(false)
{
  std::istringstream s("GET " + url + " HTTP/1.1\r\n");
  parse(s);
}

void httpRequest::parse(std::istream& in)
{
  httpMessage::parse(in);
  if (in)
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

    log_info("LANG=" << lang);
    lang_init = true;
  }

  return lang;
}

////////////////////////////////////////////////////////////////////////
// httpReply
//
httpReply::httpReply(std::ostream& s)
  : contentType("text/html"),
    socket(s),
    current_outstream(&outstream)
{ }

void httpReply::sendReply(unsigned ret)
{
  if (!isDirectMode())
  {
    log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion() << ' ' << ret << " OK");
    socket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion() << ' ' << ret << " OK" << "\r\n";

    sendHeaders(true);

    socket << "\r\n";
    if (getMethod() == "HEAD")
      log_debug("HEAD-request - empty body");
    else
    {
      log_debug("send " << outstream.str().size() << " bytes body, method=" << getMethod());
      socket << outstream.str();
    }
  }

  socket.flush();
}

void httpReply::sendHeaders(bool keepAlive)
{
  if (header.find(Date) == header.end())
    setHeader(Date, htdate(time(0)));

  if (header.find(Server) == header.end())
    setHeader(Server, ServerName);

  if (keepAlive)
  {
    if (header.find(Connection) == header.end())
      setHeader(Connection, Connection_Keep_Alive);

    if (header.find(KeepAlive) == header.end())
      setHeader(KeepAlive, KeepAliveParam);

    if (header.find(Content_Length) == header.end())
      setContentLengthHeader(outstream.str().size());
  }

  if (header.find(Content_Type) == header.end())
    setHeader(Content_Type, contentType);

  for (header_type::const_iterator it = header.begin();
       it != header.end(); ++it)
  {
    log_debug(it->first << ' ' << it->second);
    socket << it->first << ' ' << it->second << "\r\n";
  }
}

void httpReply::setMd5Sum()
{
  cxxtools::md5stream md5;
  md5 << outstream.str().size();
  setHeader(Content_MD5, md5.getHexDigest());
}

void httpReply::throwError(unsigned errorCode, const std::string& errorMessage) const
{
  throw httpError(errorCode, errorMessage);
}

void httpReply::throwError(const std::string& errorMessage) const
{
  throw httpError(errorMessage);
}

void httpReply::throwNotFound(const std::string& errorMessage) const
{
  throw notFoundException(errorMessage);
}

void httpReply::setDirectMode(bool keepAlive)
{
  if (!isDirectMode())
  {
    log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << " 200 OK");

    socket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << " 200 OK\r\n";

    sendHeaders(keepAlive);

    socket << "\r\n";
    if (getMethod() == "HEAD")
      log_debug("HEAD-request - empty body");
    else
    {
      log_debug("send " << outstream.str().size() << " bytes body");
      socket << outstream.str();
      current_outstream = &socket;
    }
  }
}

void httpReply::setDirectModeNoFlush()
{
  current_outstream = &socket;
}

////////////////////////////////////////////////////////////////////////
// httpError
//

static std::string httpErrorFormat(unsigned errcode, const std::string& msg)
{
  std::string ret;
  ret.reserve(4 + msg.size());
  char d[3];
  d[2] = '0' + errcode % 10;
  errcode /= 10;
  d[1] = '0' + errcode % 10;
  errcode /= 10;
  d[0] = '0' + errcode % 10;
  ret.assign(d, d+3);
  ret += ' ';
  ret += msg;
  return ret;
}

httpError::httpError(unsigned errcode, const std::string& msg)
  : msg(httpErrorFormat(errcode, msg))
{
}

}
