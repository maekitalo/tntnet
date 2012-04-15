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
  Job::Job(Tntnet& app_, const SocketIf* socketIf_)
    : keepAliveCounter(TntConfig::it().keepAliveMax),
      request(app_, socketIf_),
      parser(request),
      lastAccessTime(0)
    { }

  Job::~Job()
  { }

  void Job::clear()
  {
    parser.reset();
    request.clear();
  }

  int Job::msecToTimeout(time_t currentTime) const
  {
    return (lastAccessTime - currentTime + 1) * 1000
         + TntConfig::it().keepAliveTimeout
         - TntConfig::it().socketReadTimeout;
  }

  ////////////////////////////////////////////////////////////////////////
  // Tcpjob
  //

  std::string Tcpjob::getPeerIp() const
  {
    return socket.getPeerAddr();
  }

  std::string Tcpjob::getServerIp() const
  {
    return socket.getSockAddr();
  }

  bool Tcpjob::isSsl() const
  {
    return false;
  }

  void Tcpjob::accept()
  {
    socket.accept(listener);
    log_debug("connection accepted from " << getPeerIp());
  }

  void Tcpjob::regenerateJob()
  {
    if (Tntnet::shouldStop())
      queue.put(this);
    else
      queue.put(new Tcpjob(getRequest().getApplication(), listener, queue));
  }

  std::iostream& Tcpjob::getStream()
  {
    if (!socket.isConnected())
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

    return socket;
  }

  int Tcpjob::getFd() const
  {
    return socket.getFd();
  }

  void Tcpjob::setRead()
  {
    socket.setTimeout(TntConfig::it().socketReadTimeout);
  }

  void Tcpjob::setWrite()
  {
    socket.setTimeout(TntConfig::it().socketWriteTimeout);
  }

#ifdef USE_SSL
  ////////////////////////////////////////////////////////////////////////
  // SslTcpjob
  //

  std::string SslTcpjob::getPeerIp() const
  {
    return socket.getPeerAddr();
  }

  std::string SslTcpjob::getServerIp() const
  {
    return socket.getSockAddr();
  }

  bool SslTcpjob::isSsl() const
  {
    return true;
  }

  void SslTcpjob::accept()
  {
    socket.accept(listener, cxxtools::net::TcpSocket::DEFER_ACCEPT);
  }

  void SslTcpjob::handshake()
  {
    socket.handshake(listener);

    fcntl(socket.getFd(), F_SETFD, FD_CLOEXEC);

    setRead();
  }

  void SslTcpjob::regenerateJob()
  {
    if (Tntnet::shouldStop())
      queue.put(this);
    else
      queue.put(new SslTcpjob(getRequest().getApplication(), listener, queue));
  }

  std::iostream& SslTcpjob::getStream()
  {
    if (!socket.isConnected())
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

    return socket;
  }

  int SslTcpjob::getFd() const
  {
    return socket.getFd();
  }

  void SslTcpjob::setRead()
  {
    socket.setTimeout(TntConfig::it().socketReadTimeout);
  }

  void SslTcpjob::setWrite()
  {
    socket.setTimeout(TntConfig::it().socketWriteTimeout);
  }

#endif // USE_SSL

  //////////////////////////////////////////////////////////////////////
  // Jobqueue
  //
  void Jobqueue::put(JobPtr j, bool force)
  {
    j->touch();

    cxxtools::MutexLock lock(mutex);

    if (!force && capacity > 0)
    {
      while (jobs.size() >= capacity)
      {
        log_warn("Jobqueue full");
        notFull.wait(lock);
      }
    }

    jobs.push_back(j);

    if (waitThreads == 0)
      noWaitThreads.signal();

    notEmpty.signal();
  }

  Jobqueue::JobPtr Jobqueue::get()
  {
    cxxtools::MutexLock lock(mutex);

    // wait, until a job is available
    ++waitThreads;

    while (jobs.empty())
      notEmpty.wait(lock);

    --waitThreads;

    // take next job (queue is locked)
    JobPtr j = jobs.front();
    jobs.pop_front();

    // if there are threads waiting, wake another
    if (!jobs.empty() && waitThreads > 0)
      notEmpty.signal();

    notFull.signal();

    return j;
  }

}
