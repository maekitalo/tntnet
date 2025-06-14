/*
 * Copyright (C) 2012 Tommi Maekitalo
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


#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/ecpp.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/net/uri.h>
#include <cxxtools/log.h>
#include <cxxtools/split.h>
#include <strings.h>
#include <vector>
#include <iterator>

log_define("tntnet.proxy")

namespace tnt
{
class Urlmapper;
class Comploader;

////////////////////////////////////////////////////////////////////////
// component declaration
//
class Proxy : public tnt::EcppComponent
{
    friend class ProxyFactory;

public:
    Proxy(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
      : EcppComponent(ci, um, cl)
      { }

    virtual unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

////////////////////////////////////////////////////////////////////////
// factory
//
class ProxyFactory : public tnt::ComponentFactory
{
public:
    ProxyFactory(const std::string& componentName)
      : tnt::ComponentFactory(componentName)
      { }

    virtual tnt::Component* doCreate(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);
};

tnt::Component* ProxyFactory::doCreate(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
  { return new Proxy(ci, um, cl); }

static ProxyFactory proxyFactory("proxy");

////////////////////////////////////////////////////////////////////////
// component definition
//
unsigned Proxy::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
{
    std::string uriStr = request.getArg("uri");
    if (uriStr.empty())
        uriStr = request.getArg(0);  // for compatibility

    if (uriStr.empty())
    {
        log_error("proxy uri missing in configuration");
        throw std::runtime_error("proxy uri missing in configuration");
    }

    TNT_THREAD_COMPONENT_VAR(std::string, currentProxyHost, ());
    TNT_THREAD_COMPONENT_VAR(unsigned short, currentProxyPort, ());
    TNT_THREAD_COMPONENT_VAR(cxxtools::http::Client, client, ());

    cxxtools::net::Uri uri(uriStr);
    std::string pathInfo = request.getPathInfo();
    if (!pathInfo.empty() && pathInfo[0] == '/')
        pathInfo.erase(0, 1);

    std::string url = uri.path();
    if (!url.empty())
        url += '/';
    url += pathInfo;

    if (!request.getQueryString().empty())
    {
        url += '?';
        url += request.getQueryString();
    }

    if (currentProxyHost != uri.host() || currentProxyPort != uri.port())
    {
        log_debug("connect to " << uri.host() << ':' << uri.port());
        client.connect(uri.host(), uri.port());
        currentProxyHost = uri.host();
        currentProxyPort = uri.port();
    }
    else
      log_debug("already connected to " << uri.host() << ':' << uri.port());

    log_info("get file <" << url << "> qparam=" << qparam.getUrl() << " from " << uri.host() << ':' << uri.port() << " body size=" << request.getBody().size());

    // set up client request
    cxxtools::http::Request clientRequest(url);

    clientRequest.method(request.getMethod());

    clientRequest.body() << request.getBody();

    const char* value;

    clientRequest.setHeader ("Host", uri.host().c_str());

    std::string peerIp = request.getPeerIp();
    clientRequest.setHeader ("X-Real-IP", peerIp.c_str());
    clientRequest.addHeader ("X-Forwarded-For", peerIp.c_str());

    value = request.getHeader(tnt::httpheader::contentType, 0);
    if (value)
      clientRequest.header().setHeader("Content-Type", value);

    value = request.getHeader(tnt::httpheader::cookie, 0);
    if (value)
      clientRequest.header().setHeader("Cookie", value);

    value = request.getHeader(tnt::httpheader::authorization, 0);
    if (value)
      clientRequest.header().setHeader("Authorization", value);

    value = request.getHeader(tnt::httpheader::ifModifiedSince, 0);
    if (value)
      clientRequest.header().setHeader("If-Modified-Since", value);

    std::vector<std::string> forwardHeaders;
    log_debug("forwardHeaders: \"" << request.getArg("forwardHeaders") << '"');
    cxxtools::split(',', request.getArg("forwardHeaders"), std::back_inserter(forwardHeaders));
    for (unsigned n = 0; n < forwardHeaders.size(); ++n)
    {
        log_debug("forward header \"" << forwardHeaders[n] << '"');
        value = request.getHeader((forwardHeaders[n] + ':').c_str(), 0);
        if (value)
        {
            log_debug("value \"" << value << '"');
            clientRequest.header().setHeader(forwardHeaders[n].c_str(), value);
        }
    }

    // execute client request
    client.execute(clientRequest);

    // retrieve result
    std::string body = client.readBody();

    unsigned httpReturnCode = client.header().httpReturnCode();
    const char* contentType = client.header().getHeader("Content-type");

    for (cxxtools::http::MessageHeader::const_iterator it = client.header().begin();
        it != client.header().end(); ++it)
    {
        if (strcasecmp(it->first, "Set-Cookie") == 0)
        {
            log_info("cookie: " << it->first << " value: " << it->second);
            reply.setHeader(tnt::httpheader::setCookie, it->second, false);
        }
    }

    value = client.header().getHeader ("p3p");
    if (value)
      reply.setHeader("p3p:", value);

    value = client.header().getHeader("Cache-Control");
    if (value)
      reply.setHeader(tnt::httpheader::cacheControl, value);

    value = client.header().getHeader("Expires");
    if (value)
      reply.setHeader(tnt::httpheader::expires, value);

    value = client.header().getHeader("Last-Modified");
    if (value)
      reply.setHeader(tnt::httpheader::lastModified, value);

    value = client.header().getHeader("Location");
    if (value)
      reply.setHeader(tnt::httpheader::location, value);

    value = client.header().getHeader("Content-Disposition");
    if (value)
      reply.setHeader("Content-Disposition:", value);

    value = client.header().getHeader("WWW-Authenticate");
    if (value)
      reply.setHeader(tnt::httpheader::wwwAuthenticate, value);

    log_debug("got file " << request.getPathInfo() << " with http return code=" << httpReturnCode << " content type=" << (contentType ? contentType : "NULL"));

    if (httpReturnCode == HTTP_NOT_FOUND)
      return DECLINED;

    if (contentType)
      reply.setContentType(contentType);

    reply.out() << body;

    return httpReturnCode;
}
}

