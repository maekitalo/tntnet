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


#include "tnt/tntnet.h"
#include "tntnetimpl.h"

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // Tntnet
  //
  Tntnet::Tntnet()
    : _impl(new TntnetImpl())
  {
    _impl->addRef();
  }

  Tntnet::Tntnet(const Tntnet& t)
    : _impl(t._impl)
  {
    if (_impl)
      _impl->addRef();
  }

  Tntnet::Tntnet(const TntConfig& config)
    : _impl(new TntnetImpl())
  {
    _impl->addRef();
    init(config);
  }

  Tntnet& Tntnet::operator= (const Tntnet& t)
  {
    if (_impl == t._impl)
      return *this;

    if (_impl && _impl->release() == 0)
      delete _impl;

    _impl = t._impl;

    if (_impl)
      _impl->addRef();

    return *this;
  }

  void Tntnet::init(const TntConfig& config)
  {
    _impl->init(*this, config);
  }

  void Tntnet::listen(const std::string& ip, unsigned short int port)
  {
    _impl->listen(*this, ip, port);
  }

  void Tntnet::sslListen(const std::string& ipaddr, unsigned short int port,
                         const std::string& certificateFile, const std::string& keyFile,
                         int sslVerifyLevel, const std::string& sslCa)
  {
    _impl->sslListen(*this, ipaddr, port, certificateFile, keyFile, sslVerifyLevel, sslCa);
  }

  void Tntnet::run()
  {
    _impl->run();
  }

  unsigned Tntnet::getMinThreads() const
  {
    return _impl->getMinThreads();
  }

  void Tntnet::setMinThreads(unsigned n)
  {
    _impl->setMinThreads(n);
  }

  unsigned Tntnet::getMaxThreads() const
  {
    return _impl->getMaxThreads();
  }

  void Tntnet::setMaxThreads(unsigned n)
  {
    _impl->setMaxThreads(n);
  }

  void Tntnet::shutdown()
  {
    TntnetImpl::shutdown();
  }

  bool Tntnet::shouldStop()
  {
    return TntnetImpl::shouldStop();
  }

  Mapping& Tntnet::mapUrl(const std::string& url, const std::string& ci)
  {
    return _impl->mapUrl(url, Maptarget(ci));
  }

  Mapping& Tntnet::mapUrl(const std::string& url, const Compident& ci)
  {
    return _impl->mapUrl(url, Maptarget(ci));
  }

  void Tntnet::mapUrl(const std::string& url, const std::string& pathinfo, const std::string& ci)
  {
    _impl->mapUrl(url, Maptarget(ci)).setPathInfo(pathinfo);
  }

  Mapping& Tntnet::mapUrl(const std::string& url, const Maptarget& ci)
  {
    return _impl->mapUrl(url, ci);
  }

  Mapping& Tntnet::vMapUrl(const std::string& vhost, const std::string& url, const Maptarget& ci)
  {
    return _impl->vMapUrl(vhost, url, ci);
  }

  void Tntnet::setAppName(const std::string& appname)
  {
    _impl->setAppName(appname);
  }

  /// Get the app name &ndash; for details see setAppName()
  const std::string& Tntnet::getAppName() const
  {
    return _impl->getAppName();
  }

  void Tntnet::setAccessLog(const std::string& logfile_path)
  {
    _impl->setAccessLog(logfile_path);
  }

}

