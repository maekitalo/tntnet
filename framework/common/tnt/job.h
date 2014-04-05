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


#ifndef TNT_JOB_H
#define TNT_JOB_H

#include <deque>
#include <tnt/httprequest.h>
#include <tnt/httpparser.h>
#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>

/*
// in tntnet (mainthread):

\code
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
\endcode
*/

namespace tnt
{
  class Tntnet;

  /// @cond internal
  class Job : public cxxtools::RefCounted // one per request
  {
    private:
      unsigned _keepAliveCounter;

      HttpRequest _request;
      HttpRequest::Parser _parser;
      time_t _lastAccessTime;

    public:
      explicit Job(Tntnet& app, const SocketIf* socketIf = 0);
      virtual ~Job();

      virtual std::iostream& getStream() = 0;
      virtual int getFd() const = 0;
      virtual void setRead() = 0;
      virtual void setWrite() = 0;

      HttpRequest& getRequest()        { return _request; }
      HttpRequest::Parser& getParser() { return _parser; }

      unsigned decrementKeepAliveCounter()
        { return _keepAliveCounter > 0 ? --_keepAliveCounter : 0; }
      void clear();
      void touch() { time(&_lastAccessTime); }
      int msecToTimeout(time_t currentTime) const;
  };

  class Jobqueue // one per process
  {
    public:
      typedef cxxtools::SmartPtr<Job> JobPtr;

      cxxtools::Condition noWaitThreads;

    private:
      std::deque<JobPtr> _jobs;
      cxxtools::Mutex _mutex;
      cxxtools::Condition _notEmpty;
      cxxtools::Condition _notFull;
      unsigned _waitThreads;
      unsigned _capacity;

    public:
      explicit Jobqueue(unsigned capacity = 1000)
        : _waitThreads(0),
          _capacity(capacity)
        { }

      void put(JobPtr& j, bool force = false);
      JobPtr get();

      void setCapacity(unsigned c)
        { _capacity = c; }
      unsigned getCapacity() const
        { return _capacity; }
      unsigned getWaitThreadCount() const
        { return _waitThreads; }
      bool empty() const
        { return _jobs.empty(); }
  };
  /// @endcond internal
}

#endif // TNT_JOB_H

