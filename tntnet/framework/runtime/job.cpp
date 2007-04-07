/* job.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "tnt/job.h"
#include "tnt/tntnet.h"
#include <tnt/httpreply.h>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

log_define("tntnet.job")

namespace tnt
{
  unsigned Job::socket_read_timeout = 200;
  unsigned Job::socket_write_timeout = 10000;
  unsigned Job::keepalive_max = 1000;
  unsigned Job::socket_buffer_size = 16384;

  Job::~Job()
  { }

  void Job::clear()
  {
    parser.reset();
    request.clear();
    touch();
  }

  int Job::msecToTimeout(time_t currentTime) const
  {
    return (lastAccessTime - currentTime + 1) * 1000
         + getKeepAliveTimeout()
         - getSocketReadTimeout();
  }

  unsigned Job::getKeepAliveTimeout()
  {
    return HttpReply::getKeepAliveTimeout();
  }

  ////////////////////////////////////////////////////////////////////////
  // Tcpjob
  //
  void Tcpjob::accept()
  {
    log_debug("accept");
    socket.accept(listener);

    struct sockaddr_storage s = socket.getSockAddr();
    struct sockaddr_storage sockaddr;
    memcpy(&sockaddr, &s, sizeof(sockaddr));

    char buffer[INET6_ADDRSTRLEN];
    log_debug("connection accepted from "
      << inet_ntop(AF_INET6, &(socket.getPeeraddr()), buffer, sizeof(buffer)));

    getRequest().setPeerAddr(socket.getPeeraddr());
    getRequest().setServerAddr(sockaddr);
    getRequest().setSsl(false);
  }

  std::iostream& Tcpjob::getStream()
  {
    if (socket.cxxtools::net::Socket::bad())
    {
      accept();
      log_debug("connection accepted");
      if (!Tntnet::shouldStop())
        queue.put(new Tcpjob(listener, queue));
      else
        log_warn("tntnet stopping - no new job is generated");
    }
    return socket;
  }

  int Tcpjob::getFd() const
  {
    return socket.getFd();
  }

  void Tcpjob::setRead()
  {
    socket.setTimeout(getSocketReadTimeout());
  }

  void Tcpjob::setWrite()
  {
    socket.setTimeout(getSocketWriteTimeout());
  }

#ifdef USE_SSL
  ////////////////////////////////////////////////////////////////////////
  // SslTcpjob
  //
  void SslTcpjob::accept()
  {
    log_debug("accept (ssl)");
    socket.accept(listener);
    log_debug("connection accepted (ssl)");

    struct sockaddr_storage s = socket.getSockAddr();
    struct sockaddr_storage sockaddr;
    memcpy(&sockaddr, &s, sizeof(sockaddr));

    getRequest().setPeerAddr(socket.getPeeraddr());
    getRequest().setServerAddr(sockaddr);
    getRequest().setSsl(true);

    setRead();
  }

  std::iostream& SslTcpjob::getStream()
  {
    if (socket.cxxtools::net::Socket::bad())
    {
      accept();
      log_debug("connection accepted");
      if (!Tntnet::shouldStop())
        queue.put(new SslTcpjob(listener, queue));
      else
        log_warn("tntnet stopping - no new ssl-job is generated");
    }
    return socket;
  }

  int SslTcpjob::getFd() const
  {
    return socket.getFd();
  }

  void SslTcpjob::setRead()
  {
    socket.setTimeout(getSocketReadTimeout());
  }

  void SslTcpjob::setWrite()
  {
    socket.setTimeout(getSocketWriteTimeout());
  }

#endif // USE_SSL

#ifdef USE_GNUTLS
  ////////////////////////////////////////////////////////////////////////
  // GnuTlsTcpjob
  //
  void GnuTlsTcpjob::accept()
  {
    log_debug("accept (ssl)");
    accept();
    log_debug("connection accepted (ssl)");

    struct sockaddr_storage s = socket.getSockAddr();
    struct sockaddr_storage sockaddr;
    memcpy(&sockaddr, &s, sizeof(sockaddr));

    getRequest().setPeerAddr(socket.getPeeraddr());
    getRequest().setServerAddr(sockaddr);
    getRequest().setSsl(true);

    setRead();
  }

  std::iostream& GnuTlsTcpjob::getStream()
  {
    if (socket.cxxtools::net::Socket::bad())
    {
      accept();
      log_debug("connection accepted");
      if (!Tntnet::shouldStop())
        queue.put(new GnuTlsTcpjob(listener, queue));
      else
        log_warn("tntnet stopping - no new ssl-job is generated");
    }
    return socket;
  }

  int GnuTlsTcpjob::getFd() const
  {
    return socket.getFd();
  }

  void GnuTlsTcpjob::setRead()
  {
    socket.setTimeout(getSocketReadTimeout());
  }

  void GnuTlsTcpjob::setWrite()
  {
    socket.setTimeout(getSocketWriteTimeout());
  }

#endif // USE_GNUTLS

  //////////////////////////////////////////////////////////////////////
  // Jobqueue
  //
  void Jobqueue::put(JobPtr j)
  {
    log_debug("Jobqueue::put");
    j->touch();

    cxxtools::MutexLock lock(mutex);

    if (capacity > 0)
    {
      while (jobs.size() >= capacity)
      {
        log_warn("Jobqueue full");
        notFull.wait(lock);
      }
    }

    log_debug("jobs.push");
    jobs.push_back(j);

    if (waitThreads == 0)
    {
      log_info("no waiting threads left");
      noWaitThreads.signal();
    }

    notEmpty.signal();
  }

  Jobqueue::JobPtr Jobqueue::get()
  {
    cxxtools::MutexLock lock(mutex);

    // wait, until a job is available
    ++waitThreads;

    log_debug("wait for job");

    while (jobs.empty())
      notEmpty.wait(lock);

    --waitThreads;

    log_debug("Jobqueue: fetch job " << waitThreads << " waiting threads left; " << jobs.size() << " jobs in queue");

    // take next job (queue is locked)
    JobPtr j = jobs.front();
    jobs.pop_front();

    // if there are threads waiting, wake another
    if (!jobs.empty() && waitThreads > 0)
    {
      log_debug("signal another thread");
      notEmpty.signal();
    }
    notFull.signal();

    return j;
  }

}
