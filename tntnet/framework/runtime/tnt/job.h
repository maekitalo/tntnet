/* tnt/job.h
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


#ifndef TNT_JOB_H
#define TNT_JOB_H

#include <config.h>
#include <deque>
#include <cxxtools/thread.h>
#include <cxxtools/tcpstream.h>
#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <tnt/pointer.h>
#include <time.h>
#include "tnt/ssl.h"

/**
// in tntnet (mainthread):
Jobqueue queue;
void mainloop()
{
  while (1)
  {
    Jobqueue::JobPtr j = new Tcpjob();
    j->accept(poller.get());
    queue.put(j);
  }
}

// in server (workerthread):
void Server::run()
{
  while (1)
  {
    Jobqueue::JobPtr j = queue.get();
    std::iostream& socket = j->getStream();
    processRequest(socket);
  }
}
*/

namespace tnt
{
  /** Job - one per request */
  class Job
  {
      unsigned keepAliveCounter;

      HttpRequest request;
      HttpMessage::Parser parser;
      time_t lastAccessTime;

      unsigned refs;
      cxxtools::Mutex mutex;

      static unsigned socket_read_timeout;
      static unsigned socket_write_timeout;
      static unsigned keepalive_max;
      static unsigned socket_buffer_size;

    public:
      Job()
        : keepAliveCounter(keepalive_max),
          parser(request),
          lastAccessTime(0),
          refs(0)
        { }

    protected:
      virtual ~Job();

    public:
      unsigned addRef();
      unsigned release();

      virtual std::iostream& getStream() = 0;
      virtual int getFd() const = 0;
      virtual void setRead() = 0;
      virtual void setWrite() = 0;

      HttpRequest& getRequest()         { return request; }
      HttpMessage::Parser& getParser()  { return parser; }

      unsigned decrementKeepAliveCounter()
        { return keepAliveCounter > 0 ? --keepAliveCounter : 0; }
      void clear();
      void touch()     { time(&lastAccessTime); }
      int msecToTimeout(time_t currentTime) const;

      static void setSocketReadTimeout(unsigned ms)     { socket_read_timeout = ms; }
      static void setSocketWriteTimeout(unsigned ms)    { socket_write_timeout = ms; }
      static void setKeepAliveMax(unsigned n)       { keepalive_max = n; }
      static void setSocketBufferSize(unsigned b)   { socket_buffer_size = b; }

      static unsigned getSocketReadTimeout()        { return socket_read_timeout; }
      static unsigned getSocketWriteTimeout()       { return socket_write_timeout; }
      static unsigned getKeepAliveTimeout();
      static unsigned getKeepAliveMax()       { return keepalive_max; }
      static unsigned getSocketBufferSize()   { return socket_buffer_size; }
  };

  class Jobqueue;

  class Tcpjob : public Job
  {
      cxxtools::net::iostream socket;
      const cxxtools::net::Server& listener;
      Jobqueue& queue;

      void accept();

    public:
      Tcpjob(const cxxtools::net::Server& listener_, Jobqueue& queue_)
        : socket(getSocketBufferSize(), getSocketReadTimeout()),
          listener(listener_),
          queue(queue_)
        { }

      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };

#ifdef USE_SSL
  class SslTcpjob : public Job
  {
      ssl_iostream socket;
      const SslServer& listener;
      Jobqueue& queue;

      void accept();

    public:
      SslTcpjob(const SslServer& listener_, Jobqueue& queue_)
        : socket(getSocketBufferSize(), getSocketReadTimeout()),
          listener(listener_),
          queue(queue_)
        { }

      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };
#endif // USE_SSL

  /** Jobqueue - one per process */
  class Jobqueue
  {
    public:
      typedef Pointer<Job> JobPtr;

      cxxtools::Condition noWaitThreads;

    private:
      std::deque<JobPtr> jobs;
      cxxtools::Mutex mutex;
      cxxtools::Condition notEmpty;
      cxxtools::Condition notFull;
      unsigned waitThreads;
      unsigned capacity;

    public:
      explicit Jobqueue(unsigned capacity_ = 1000)
        : waitThreads(0),
          capacity(capacity_)
        { }

      void put(JobPtr j, bool force = false);
      JobPtr get();

      void setCapacity(unsigned c)
        { capacity = c; }
      unsigned getCapacity() const
        { return capacity; }
      unsigned getWaitThreadCount() const
        { return waitThreads; }
      bool empty() const
        { return jobs.empty(); }
  };
}

#endif // TNT_JOB_H

