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

#include <tnt/httprequest.h>
#include <tnt/httpparser.h>

#include <memory>
#include <deque>
#include <mutex>
#include <condition_variable>

/// @cond internal

namespace tnt
{
class Tntnet;

class Job
{
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
    cxxtools::Milliseconds msecToTimeout(time_t currentTime) const;
};

class Jobqueue
{
    std::deque<std::unique_ptr<Job>> _jobs;
    std::mutex _mutex;
    std::condition_variable _notEmpty;
    std::condition_variable _notFull;
    unsigned _waitThreads;
    unsigned _capacity;

public:
    std::condition_variable noWaitThreads;

    explicit Jobqueue(unsigned capacity = 1000)
      : _waitThreads(0),
        _capacity(capacity)
      { }

    void put(std::unique_ptr<Job> j, bool force = false);
    void putEmpty();
    std::unique_ptr<Job> get();

    void setCapacity(unsigned c)
      { _capacity = c; }
    unsigned getCapacity() const
      { return _capacity; }
    unsigned getWaitThreadCount() const
      { return _waitThreads; }
    bool empty() const
      { return _jobs.empty(); }
};

}

#endif // TNT_JOB_H
