/* job.cpp
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

#include "tnt/job.h"
#include <tnt/httpreply.h>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

log_define("tntnet.job");

namespace tnt
{
  unsigned job::socket_read_timeout = 200;
  unsigned job::socket_write_timeout = 10000;
  unsigned job::keepalive_max = 100;
  unsigned job::socket_buffer_size = 4096;

  job::~job()
  { }

  void job::clear()
  {
    parser.reset();
    request.clear();
    touch();
  }

  int job::msecToTimeout() const
  {
    return (lastAccessTime - time(0) + 1) * 1000
         + getKeepAliveTimeout()
         - getSocketReadTimeout();
  }

  void job::setKeepAliveTimeout(unsigned ms)
  {
    httpReply::setKeepAliveTimeout(ms);
  }

  unsigned job::getKeepAliveTimeout()
  {
    return httpReply::getKeepAliveTimeout();
  }

  ////////////////////////////////////////////////////////////////////////
  // tcpjob
  //
  void tcpjob::Accept(const cxxtools::tcp::Server& listener)
  {
    log_debug("accept");
    socket.Accept(listener);

    struct sockaddr s = socket.getSockAddr();
    struct sockaddr_in sockaddr_in;
    memcpy(&sockaddr_in, &s, sizeof(sockaddr_in));

    char buffer[20];
    log_debug("connection accepted from "
      << inet_ntop(AF_INET, &(socket.getPeeraddr_in().sin_addr), buffer, sizeof(buffer)));

    getRequest().setPeerAddr(socket.getPeeraddr_in());
    getRequest().setServerAddr(sockaddr_in);
    getRequest().setSsl(false);
  }

  std::iostream& tcpjob::getStream()
  {
    return socket;
  }

  int tcpjob::getFd() const
  {
    return socket.getFd();
  }

  void tcpjob::setRead()
  {
    socket.setTimeout(getSocketReadTimeout());
  }

  void tcpjob::setWrite()
  {
    socket.setTimeout(getSocketWriteTimeout());
  }

#ifdef USE_SSL
  ////////////////////////////////////////////////////////////////////////
  // ssl_tcpjob
  //
  void ssl_tcpjob::Accept(const SslServer& listener)
  {
    log_debug("accept (ssl)");
    socket.Accept(listener);
    log_debug("connection accepted (ssl)");

    struct sockaddr s = socket.getSockAddr();
    struct sockaddr_in sockaddr_in;
    memcpy(&sockaddr_in, &s, sizeof(sockaddr_in));

    getRequest().setPeerAddr(socket.getPeeraddr_in());
    getRequest().setServerAddr(sockaddr_in);
    getRequest().setSsl(true);
  }

  std::iostream& ssl_tcpjob::getStream()
  {
    return socket;
  }

  int ssl_tcpjob::getFd() const
  {
    return socket.getFd();
  }

  void ssl_tcpjob::setRead()
  {
    socket.setTimeout(getSocketReadTimeout());
  }

  void ssl_tcpjob::setWrite()
  {
    socket.setTimeout(getSocketWriteTimeout());
  }

#endif // USE_SSL

  //////////////////////////////////////////////////////////////////////
  // jobqueue
  //
  void jobqueue::put(job_ptr j)
  {
    log_debug("jobqueue::put");
    j->touch();

    cxxtools::MutexLock lock(mutex);

    if (capacity > 0 && jobs.size() < capacity)
      jobs.push_back(j);
    else
    {
      log_warn("jobqueue full");
      notFull.Wait(lock);
      jobs.push_back(j);
    }

    if (waitThreads == 0)
    {
      log_info("no waiting threads left");
      noWaitThreads.Signal();
    }

    notEmpty.Signal();
  }

  jobqueue::job_ptr jobqueue::get()
  {
    // warten, bis ein Job vohanden ist
    ++waitThreads;

    cxxtools::MutexLock lock(mutex);
    while (jobs.empty())
      notEmpty.Wait(lock);

    --waitThreads;

    log_debug("jobqueue: fetch job " << waitThreads << " waiting threads left");

    // nehme nächste Job (lock ist noch aktiv)
    job_ptr j = jobs.front();
    jobs.pop_front();

    // wenn weitere Jobs vorhanden sind, dann wecken wir
    // einen weitern Thread
    if (!jobs.empty())
      notEmpty.Signal();
    notFull.Signal();

    return j;
  }

}
