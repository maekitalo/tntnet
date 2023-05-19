/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <tnt/httperror.h>
#include <tnt/sessionscope.h>
#include <tnt/socketif.h>
#include <tnt/stringlessignorecase.h>

#include <cxxtools/log.h>
#include <cxxtools/base64stream.h>

#include <sstream>
#include <mutex>

#include <pthread.h>
#include "config.h"
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>

namespace tnt
{
log_define("tntnet.httprequest")

////////////////////////////////////////////////////////////////////////
// HttpRequest
//
std::atomic<unsigned> HttpRequest::_nextSerial(0);

HttpRequest::HttpRequest(Tntnet& application, const SocketIf* socketIf)
  : _socketIf(socketIf),
    _encodingRead(false),
    _threadContext(0),
    _applicationScopeLocked(false),
    _sessionScopeLocked(false),
    _secureSessionScopeLocked(false),
    _application(application)
{
}

HttpRequest::HttpRequest(Tntnet& application, const std::string& url, const SocketIf* socketIf)
  : _socketIf(socketIf),
    _threadContext(0),
    _applicationScopeLocked(false),
    _sessionScopeLocked(false),
    _secureSessionScopeLocked(false),
    _application(application)
{
    std::istringstream s("GET " + url + " HTTP/1.1\r\n\r\n");
    parse(s);
}

HttpRequest::HttpRequest(HttpRequest&& r)
  : _methodLen(0),
    _pathinfo(r._pathinfo),
    _args(r._args),
    _getparam(r._getparam),
    _postparam(r._postparam),
    _qparam(r._qparam),
    _socketIf(r._socketIf),
    _ct(r._ct),
    _mp(r._mp),
    _serial(r._serial),
    _requestScope(std::move(r._requestScope)),
    _applicationScope(std::move(r._applicationScope)),
    _sessionScope(std::move(r._sessionScope)),
    _secureSessionScope(std::move(r._secureSessionScope)),
    _threadContext(r._threadContext),
    _applicationScopeLocked(false),
    _sessionScopeLocked(false),
    _secureSessionScopeLocked(false),
    _application(r._application)
{
}

HttpRequest::~HttpRequest()
{
    releaseLocks();
}

HttpRequest& HttpRequest::operator= (HttpRequest&& r)
{
    _pathinfo = r._pathinfo;
    _args = r._args;
    _getparam = r._getparam;
    _postparam = r._postparam;
    _qparam = r._qparam;
    _ct = r._ct;
    _mp = r._mp;
    _socketIf = r._socketIf;
    _serial = r._serial;
    _requestScope = std::move(r._requestScope);
    _applicationScope = std::move(r._applicationScope);
    _sessionScope = std::move(r._sessionScope);
    _secureSessionScope = std::move(r._secureSessionScope);
    _threadContext = r._threadContext;
    _applicationScopeLocked = false;
    _sessionScopeLocked = false;
    _secureSessionScopeLocked = false;

    return *this;
}

void HttpRequest::clear()
{
    HttpMessage::clear();
    _body.clear();
    _methodLen = 0;
    _method[0] = '\0';
    _url.clear();
    _queryString.clear();
    _contentSize = 0;
    _pathinfo.clear();
    _args.clear();
    _getparam.clear();
    _postparam.clear();
    _qparam.clear();
    _ct = Contenttype();
    _mp = Multipart();
    httpcookies.clear();
    _encodingRead = false;
    _username.clear();
    _password.clear();

    releaseLocks();

    _requestScope = nullptr;
    _applicationScope = nullptr;
    _sessionScope = nullptr;
    _secureSessionScope = nullptr;
    _threadContext = nullptr;
}

void HttpRequest::setMethod(const char* m)
{
    if (strlen(m) >= 7)
      throw HttpError(HTTP_BAD_REQUEST, "invalid method");
    std::strcpy(_method, m);
}

void HttpRequest::setArgs(const args_type& a, bool addToQparam)
{
    _args = a;
    for (args_type::const_iterator it = a.begin(); it != a.end(); ++it)
      _qparam.add(it->first, it->second);
}

std::string HttpRequest::getArgDef(args_type::size_type n, const std::string& def) const
{
    std::ostringstream k;
    k << "arg" << n;
    return getArg(k.str(), def);
}

std::string HttpRequest::getArg(const std::string& name, const std::string& def) const
{
    args_type::const_iterator it = _args.find(name);
    return it == _args.end() ? def : it->second;
}

void HttpRequest::parse(std::istream& in)
{
    Parser p(*this);
    p.parse(in);
    if (!p.failed())
      doPostParse();
}

void HttpRequest::doPostParse()
{
    if (hasHeader("Expect:"))
      throw HttpError(HTTP_EXPECTATION_FAILED, "expectation failed", "Expect not supported by this server");

    _getparam.parse_url(getQueryString());

    if (isMethodPOST())
    {
        std::istringstream in(getHeader(httpheader::contentType));
        in >> _ct;

        if (in)
        {
            if (_ct.isMultipart())
            {
                _mp.set(_ct.getBoundary(), getBody());
                for (Multipart::const_iterator it = _mp.begin(); it != _mp.end(); ++it)
                {
                    // don't copy uploaded files into qparam to prevent unnecessery
                    // copies of large chunks
                    if (it->getFilename().empty())
                    {
                        std::string multipartBody(it->getBodyBegin(), it->getBodyEnd());
                        _postparam.add(it->getName(), multipartBody);
                    }
                }
            }
            else if (_ct.getType() == "application"
                  && _ct.getSubtype() == "x-www-form-urlencoded")
            {
                _postparam.parse_url(getBody());
            }
        }
    }

    _qparam.add(_getparam);
    _qparam.add(_postparam);

    _serial = ++_nextSerial;
}

const Cookies& HttpRequest::getCookies() const
{
    if (!httpcookies.hasCookies())
    {
        header_type::const_iterator it = header.find(httpheader::cookie);
        if (it != header.end())
          const_cast<HttpRequest*>(this)->httpcookies.set(it->second);
    }

    return httpcookies;
}

const Encoding& HttpRequest::getEncoding() const
{
    if (!_encodingRead)
    {
        _encoding.parse(getHeader(httpheader::acceptEncoding));
        _encodingRead = true;
    }
    return _encoding;
}

const std::string& HttpRequest::getUsername() const
{
    if (_username.empty() && hasHeader(httpheader::authorization))
    {
        std::istringstream authHeader(getHeader(httpheader::authorization));
        while (authHeader && authHeader.get() != ' ')
          ;
        cxxtools::Base64istream in(authHeader);
        std::getline(in, _username, ':');
        std::getline(in, _password);
    }

    return _username;
}

const std::string& HttpRequest::getPassword() const
{
    getUsername();
    return _password;
}

bool HttpRequest::verifyPassword(const std::string& password) const
{
    getUsername();
    return _password == password;
}

bool HttpRequest::keepAlive() const
{
    // request is keep alive when either a connection header is explicitely set to keep alive
    // or the http version is 1.1
    Messageheader::const_iterator it = header.find(httpheader::connection);

    return it == header.end()
               ? getMinorVersion() >= 1 && getMajorVersion() >= 1
               : tnt::StringCompareIgnoreCase<const char*>(it->second,
                    httpheader::connectionKeepAlive) == 0;
}

const Contenttype& HttpRequest::getContentTypePriv() const
{
    std::istringstream in(getHeader(httpheader::contentType));
    in >> _ct;
    return _ct;
}

void HttpRequest::ensureApplicationScopeLock()
{
    ensureSessionScopeLock();
    if (_applicationScope && !_applicationScopeLocked)
    {
        _applicationScope->lock();
        _applicationScopeLocked = true;
    }
}

void HttpRequest::ensureSessionScopeLock()
{
    log_debug("ensureSessionScopeLock " << static_cast<void*>(_sessionScope.get()) << " locked=" << _sessionScopeLocked << " secure locked=" << _secureSessionScopeLocked);

    if (_sessionScope && !_sessionScopeLocked)
    {
        log_debug("lock session scope");
        _sessionScope->lock();
        _sessionScopeLocked = true;
    }

    if (_secureSessionScope && !_secureSessionScopeLocked)
    {
        log_debug("lock secure session scope");
        _secureSessionScope->lock();
        _secureSessionScopeLocked = true;
    }

    log_debug("session scope locked=" << _sessionScopeLocked << " secure locked=" << _secureSessionScopeLocked);
}

void HttpRequest::releaseApplicationScopeLock()
{
    releaseSessionScopeLock();

    if (_applicationScope && _applicationScopeLocked)
    {
        _applicationScopeLocked = false;
        _applicationScope->unlock();
    }
}

void HttpRequest::releaseSessionScopeLock()
{
    log_debug("releaseSessionScopeLock " << static_cast<void*>(_sessionScope.get()) << " locked=" << _sessionScopeLocked << " secure locked=" << _secureSessionScopeLocked);

    if (_secureSessionScope && _secureSessionScopeLocked)
    {
        _secureSessionScopeLocked = false;
        _secureSessionScope->unlock();
        log_debug("secure session scope unlocked");
    }

    if (_sessionScope && _sessionScopeLocked)
    {
        _sessionScopeLocked = false;
        _sessionScope->unlock();
        log_debug("session scope unlocked");
    }
}

Scope& HttpRequest::getRequestScope()
{
    if (!_requestScope)
        _requestScope.reset(new Scope());
    return *_requestScope;
}

Scope& HttpRequest::getThreadScope()
{
    if (_threadContext == 0)
        throw std::runtime_error("threadcontext not set");
    return _threadContext->getScope();
}

Scope& HttpRequest::getApplicationScope()
{
    ensureApplicationScopeLock();
    return *_applicationScope;
}

std::shared_ptr<Sessionscope> HttpRequest::getSessionScopePtr()
{
    if (!_sessionScope)
    {
        _sessionScope = std::make_shared<Sessionscope>();
        log_debug("new session scope " << static_cast<void*>(_sessionScope.get()));
    }

    ensureSessionScopeLock();
    return _sessionScope;
}

std::shared_ptr<Sessionscope> HttpRequest::getSecureSessionScopePtr()
{
    if (!_secureSessionScope)
    {
        _secureSessionScope = std::make_shared<Sessionscope>();
        log_debug("new secure session scope " << static_cast<void*>(_secureSessionScope.get()));
    }
    ensureSessionScopeLock();
    return _secureSessionScope;
}

bool HttpRequest::hasSessionScope() const
{
    return _sessionScope != 0 && !_sessionScope->empty();
}

bool HttpRequest::hasSecureSessionScope() const
{
    return _secureSessionScope != 0 && !_secureSessionScope->empty();
}

void HttpRequest::postRunCleanup()
{
#ifdef ENABLE_LOCALE
    tnt::clearLocaleCache();
#endif
}
}

