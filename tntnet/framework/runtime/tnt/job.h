/* tnt/job.h
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

#ifndef TNT_JOB_H
#define TNT_JOB_H

#include <config.h>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <cxxtools/thread.h>
#include <cxxtools/tcpstream.h>
#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <time.h>

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
  static const unsigned socket_timeout = 200;
  static const unsigned keepalive_timeout = 15000;

  /** job - one per request */
  class job
  {
      unsigned keepAliveCounter;

      httpRequest request;
      httpMessage::parser parser;
      time_t lastAccessTime;

    public:
      job()
        : parser(request),
          keepAliveCounter(10),
          lastAccessTime(0)
        { }
      virtual ~job();

      virtual std::iostream& getStream() = 0;
      virtual int getFd() const = 0;

      httpRequest& getRequest()         { return request; }
      httpMessage::parser& getParser()  { return parser; }

      bool decrementKeepAliveCounter()
        { return keepAliveCounter > 0 && --keepAliveCounter > 0; }
      void clear();
      void touch()     { time(&lastAccessTime); }
      int msecToTimeout() const;
  };

  class tcpjob : public job
  {
      cxxtools::tcp::iostream socket;

    public:
      tcpjob()
        : socket(1024, socket_timeout)
        { }

      void Accept(const cxxtools::tcp::Server& listener);

      std::iostream& getStream();
      int getFd() const;
  };

#ifdef USE_SSL
  class ssl_tcpjob : public job
  {
      ssl_iostream socket;

    public:
      ssl_tcpjob()
        : socket(1024, socket_timeout)
        { }

      void Accept(const SslServer& listener);

      std::iostream& getStream();
      int getFd() const;
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

