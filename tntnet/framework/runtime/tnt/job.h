/* tnt/job.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef TNT_JOB_H
#define TNT_JOB_H

#include <config.h>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <cxxtools/thread.h>
#include <cxxtools/tcpstream.h>

#ifdef USE_SSL
#  include "tnt/ssl.h"
#endif

/**
// in tntnet (mainthread):
jobqueue queue;
void mainloop()
{
  while (1)
  {
    jobqueue::ptr_type j = new tcpjob();
    j->Accept(poller.get());
    queue.put(j);
  }
}

// in server (workerthread):
void server::Run()
{
  while (1)
  {
    jobqueue::ptr_type j = queue.get();
    std::iostream& socket = j->getStream();
    processRequest(socket);
  }
}
*/

namespace tnt
{
  /** job - one per request */
  class job
  {
    public:
      virtual ~job() { }
      virtual std::iostream& getStream() = 0;
      virtual const struct sockaddr_in& getPeeraddr_in() const = 0;
      virtual const struct sockaddr_in& getServeraddr_in() const = 0;
      virtual bool isSsl() const = 0;
  };

  class tcpjob : public job
  {
      cxxtools::tcp::iostream socket;
      struct sockaddr_in sockaddr_in;

    public:
      tcpjob()
        : socket(1024, 15000)
        { }

      void Accept(const cxxtools::tcp::Server& listener);

      std::iostream& getStream();
      const struct sockaddr_in& getPeeraddr_in() const;
      const struct sockaddr_in& getServeraddr_in() const;
      bool isSsl() const     { return false; }
  };

#ifdef USE_SSL
  class ssl_tcpjob : public job
  {
      ssl_iostream socket;
      struct sockaddr_in sockaddr_in;

    public:
      ssl_tcpjob()
        : socket(1024, 15000)
        { }

      void Accept(const SslServer& listener);

      std::iostream& getStream();
      const struct sockaddr_in& getPeeraddr_in() const;
      const struct sockaddr_in& getServeraddr_in() const;
      bool isSsl() const     { return true; }
  };
#endif // USE_SSL

  /** jobqueue - one per process */
  class jobqueue
  {
    public:
      typedef boost::shared_ptr<job> job_ptr;
      cxxtools::Condition noWaitThreads;

    private:
      std::deque<job_ptr> jobs;
      cxxtools::Condition notEmpty;
      unsigned waitThreads;

    public:
      jobqueue()
        : waitThreads(0)
        { }
      void put(job_ptr j);
      job_ptr get();

      unsigned getWaitThreadCount()
        { return waitThreads; }
  };
}

#endif // TNT_JOB_H

