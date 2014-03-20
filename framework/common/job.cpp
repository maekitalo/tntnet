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


#include "tnt/job.h"
#include "tnt/tcpjob.h"
#include "tnt/tntnet.h"
#include <tnt/httpreply.h>
#include <tnt/ssl.h>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <config.h>
#include <fcntl.h>

log_define("tntnet.job")

namespace tnt
{
  Job::Job(Tntnet& app, const SocketIf* socketIf)
    : _keepAliveCounter(TntConfig::it().keepAliveMax),
      _request(app, socketIf),
      _parser(_request),
      _lastAccessTime(0)
    { }

  Job::~Job()
    { }

  void Job::clear()
  {
    _parser.reset();
    _request.clear();
  }

  int Job::msecToTimeout(time_t currentTime) const
  {
    return (_lastAccessTime - currentTime + 1) * 1000
         + TntConfig::it().keepAliveTimeout
         - TntConfig::it().socketReadTimeout;
  }

  ////////////////////////////////////////////////////////////////////////
  // Tcpjob
  //
  std::string Tcpjob::getPeerIp() const
  {
    return _socket.getPeerAddr();
  }

  std::string Tcpjob::getServerIp() const
  {
    return _socket.getSockAddr();
  }

  bool Tcpjob::isSsl() const
  {
    return false;
  }

  void Tcpjob::accept()
  {
    _socket.accept(_listener);
    log_debug("connection accepted from " << getPeerIp());
  }

  void Tcpjob::regenerateJob()
  {
    Jobqueue::JobPtr p;

    if (Tntnet::shouldStop())
      p = this;
    else
      p = new Tcpjob(getRequest().getApplication(), _listener, _queue);

    _queue.put(p);
  }

  std::iostream& Tcpjob::getStream()
  {
    if (!_socket.isConnected())
    {
      try
      {
        accept();
        touch();
      }
      catch (const std::exception& e)
      {
        regenerateJob();
        log_debug("exception occured in accept: " << e.what());
        throw;
      }

      regenerateJob();
    }

    return _socket;
  }

  int Tcpjob::getFd() const
  {
    return _socket.getFd();
  }

  void Tcpjob::setRead()
  {
    _socket.setTimeout(TntConfig::it().socketReadTimeout);
  }

  void Tcpjob::setWrite()
  {
    _socket.setTimeout(TntConfig::it().socketWriteTimeout);
  }

#ifdef USE_SSL
  ////////////////////////////////////////////////////////////////////////
  // SslTcpjob
  //

  std::string SslTcpjob::getPeerIp() const
  {
    return _socket.getPeerAddr();
  }

  std::string SslTcpjob::getServerIp() const
  {
    return _socket.getSockAddr();
  }

  bool SslTcpjob::isSsl() const
  {
    return true;
  }

  void SslTcpjob::accept()
  {
    _socket.accept(_listener, cxxtools::net::TcpSocket::DEFER_ACCEPT);
  }

  void SslTcpjob::handshake()
  {
    _socket.handshake(_listener);

    fcntl(_socket.getFd(), F_SETFD, FD_CLOEXEC);

    setRead();
  }

  void SslTcpjob::regenerateJob()
  {
    Jobqueue::JobPtr p;

    if (Tntnet::shouldStop())
      p = this;
    else
      p = new SslTcpjob(getRequest().getApplication(), _listener, _queue);

    _queue.put(p);
  }

  std::iostream& SslTcpjob::getStream()
  {
    if (!_socket.isConnected())
    {
      try
      {
        accept();
        touch();
      }
      catch (const std::exception& e)
      {
        log_debug("exception occured in accept (ssl): " << e.what());
        regenerateJob();
        throw;
      }

      regenerateJob();

      if (!Tntnet::shouldStop())
        handshake();
    }

    return _socket;
  }

  int SslTcpjob::getFd() const
  {
    return _socket.getFd();
  }

  void SslTcpjob::setRead()
  {
    _socket.setTimeout(TntConfig::it().socketReadTimeout);
  }

  void SslTcpjob::setWrite()
  {
    _socket.setTimeout(TntConfig::it().socketWriteTimeout);
  }

#endif // USE_SSL

  //////////////////////////////////////////////////////////////////////
  // Jobqueue
  //
  void Jobqueue::put(JobPtr& j, bool force)
  {
    j->touch();

    cxxtools::MutexLock lock(_mutex);

    if (!force && _capacity > 0)
    {
      while (_jobs.size() >= _capacity)
      {
        log_warn("Jobqueue full");
        _notFull.wait(lock);
      }
    }

    _jobs.push_back(j);
    // We have to drop ownership before releasing the lock of the queue.
    // Therefore we set the smart pointer to 0.
    j = 0;

    if (_waitThreads == 0)
      noWaitThreads.signal();

    _notEmpty.signal();
  }

  Jobqueue::JobPtr Jobqueue::get()
  {
    cxxtools::MutexLock lock(_mutex);

    // wait, until a job is available
    ++_waitThreads;

    while (_jobs.empty())
      _notEmpty.wait(lock);

    --_waitThreads;

    // take next job (queue is locked)
    JobPtr j = _jobs.front();
    _jobs.pop_front();

    // if there are threads waiting, wake another
    if (!_jobs.empty() && _waitThreads > 0)
      _notEmpty.signal();

    _notFull.signal();

    return j;
  }
}

