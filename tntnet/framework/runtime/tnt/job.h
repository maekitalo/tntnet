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
  /** job - one per request */
  class job
  {
      unsigned keepAliveCounter;

      httpRequest request;
      httpMessage::parser parser;
      time_t lastAccessTime;

      static unsigned socket_timeout;
      static unsigned keepalive_max;
      static unsigned socket_buffer_size;

    public:
      job()
        : parser(request),
          keepAliveCounter(keepalive_max),
          lastAccessTime(0)
        { }
      virtual ~job();

      virtual std::iostream& getStream() = 0;
      virtual int getFd() const = 0;

      httpRequest& getRequest()         { return request; }
      httpMessage::parser& getParser()  { return parser; }

      unsigned decrementKeepAliveCounter()
        { return keepAliveCounter > 0 ? --keepAliveCounter : 0; }
      void clear();
      void touch()     { time(&lastAccessTime); }
      int msecToTimeout() const;

      static void setSocketTimeout(unsigned ms)     { socket_timeout = ms; }
      static void setKeepAliveTimeout(unsigned ms);
      static void setKeepAliveMax(unsigned n)       { keepalive_max = n; }
      static void setSocketBufferSize(unsigned b)   { socket_buffer_size = b; }

      static unsigned getSocketTimeout()      { return socket_timeout; }
      static unsigned getKeepAliveTimeout();
      static unsigned getKeepAliveMax()       { return keepalive_max; }
      static unsigned getSocketBufferSize()   { return socket_buffer_size; }
  };

  class tcpjob : public job
  {
      cxxtools::tcp::iostream socket;

    public:
      tcpjob()
        : socket(getSocketBufferSize(), getSocketTimeout())
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
        : socket(getSocketBufferSize(), getSocketTimeout())
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
      cxxtools::Mutex mutex;
      cxxtools::Condition notEmpty;
      cxxtools::Condition notFull;
      unsigned waitThreads;
      unsigned capacity;

    public:
      explicit jobqueue(unsigned capacity_)
        : waitThreads(0),
          capacity(capacity_)
        { }

      void put(job_ptr j);
      job_ptr get();

      void setCapacity(unsigned c)
        { capacity = c; }
      unsigned getWaitThreadCount() const
        { return waitThreads; }
  };
}

#endif // TNT_JOB_H

