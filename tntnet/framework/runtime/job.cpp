/* job.cpp
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

#include "tnt/job.h"
#include "tnt/log.h"

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // tcpjob
  //
  void tcpjob::Accept(const cxxtools::tcp::Server& listener)
  {
    log_debug("accept");
    socket.Accept(listener);
    log_debug("connection accepted");

    struct sockaddr s = socket.getSockAddr();
    memcpy(&sockaddr_in, &s, sizeof(sockaddr_in));
  }

  const struct sockaddr_in& tcpjob::getPeeraddr_in() const
  {
    return socket.getPeeraddr_in();
  }

  const struct sockaddr_in& tcpjob::getServeraddr_in() const
  {
    return sockaddr_in;
  }

  std::iostream& tcpjob::getStream()
  {
    return socket;
  }

  ////////////////////////////////////////////////////////////////////////
  // ssl_tcpjob
  //
  void ssl_tcpjob::Accept(const SslServer& listener)
  {
    log_debug("accept (ssl)");
    socket.Accept(listener);
    log_debug("connection accepted (ssl)");

    struct sockaddr s = socket.getSockAddr();
    memcpy(&sockaddr_in, &s, sizeof(sockaddr_in));
  }

  const struct sockaddr_in& ssl_tcpjob::getPeeraddr_in() const
  {
    return socket.getPeeraddr_in();
  }

  const struct sockaddr_in& ssl_tcpjob::getServeraddr_in() const
  {
    return sockaddr_in;
  }

  std::iostream& ssl_tcpjob::getStream()
  {
    return socket;
  }

  //////////////////////////////////////////////////////////////////////
  // jobqueue
  //
  log_define_class(jobqueue, "tntnet.jobqueue");

  void jobqueue::put(job_ptr j)
  {
    log_debug("jobqueue::put");
    cxxtools::MutexLock lock(notEmpty);
    jobs.push_back(j);
    notEmpty.Signal();
  }

  jobqueue::job_ptr jobqueue::get()
  {
    // warten, bis ein Job vohanden ist
    ++waitThreads;

    cxxtools::MutexLock lock(notEmpty);
    while (jobs.empty())
      notEmpty.Wait(lock);

    if (--waitThreads == 0)
    {
      log_warn("no waiting threads left");
      noWaitThreads.Signal();
    }

    log_debug("jobqueue: fetch job " << waitThreads << " waiting Threads left");

    // nehme nächste Job (lock ist noch aktiv)
    job_ptr j = jobs.front();
    jobs.pop_front();

    // wenn weitere Jobs vorhanden sind, dann wecken wir
    // einen weitern Thread
    if (!jobs.empty())
      notEmpty.Signal();

    return j;
  }

}
