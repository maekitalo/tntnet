/*
 * Copyright (C) 2005-2006 Tommi Maekitalo
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


#include "tnt/poller.h"
#include "tnt/pollerimpl.h"
#include "tnt/tntnet.h"
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>
#include <ios>
#include <unistd.h>
#include <fcntl.h>
#ifdef WITH_EPOLL
#include <sys/epoll.h>
#include <errno.h>
#endif

log_define("tntnet.poller")

namespace tnt
{
  PollerIf::~PollerIf() { }

  Poller::Poller(Jobqueue& q)
    : _impl(new PollerImpl(q))
    { }

  void Poller::run()
    { _impl->run(); }

#ifdef WITH_EPOLL

  PollerImpl::PollerImpl(Jobqueue& q)
    : _queue(q),
      _pollFd(-1)
  {
    _pollFd = ::epoll_create(256);
    if (_pollFd < 0)
      throw cxxtools::SystemError("epoll_create");

    fcntl(_notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);
    addFd(_notify_pipe.getReadFd());
  }

  PollerImpl::~PollerImpl()
    { close(_pollFd); }

  void PollerImpl::addFd(int fd)
  {
    epoll_event e;
    e.events = EPOLLIN;
    e.data.fd = fd;
    int ret = ::epoll_ctl(_pollFd, EPOLL_CTL_ADD, fd, &e);
    if (ret < 0)
      throw cxxtools::SystemError("epoll_ctl(EPOLL_CTL_ADD)");
  }

  bool PollerImpl::removeFd(int fd)
  {
    epoll_event e;
    e.data.fd = fd;
    int ret = ::epoll_ctl(_pollFd, EPOLL_CTL_DEL, fd, &e);
    if (ret < 0)
    {
      if (errno == EBADF || errno == ENOENT)
        return false;
      else
        throw cxxtools::SystemError("epoll_ctl(EPOLL_CTL_DEL)");
    }

    return true;
  }

  void PollerImpl::doStop()
    { _notify_pipe.write('A'); }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    cxxtools::MutexLock lock(_mutex);
    _new_jobs.push_back(job);
    _notify_pipe.write('A');
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(_mutex);
    if (!_new_jobs.empty())
    {
      // append new jobs to current
      time_t currentTime;
      time(&currentTime);
      for (new_jobs_type::iterator it = _new_jobs.begin();
           it != _new_jobs.end(); ++it)
      {
        addFd((*it)->getFd());
        _jobs[(*it)->getFd()] = *it;

        int msec = (*it)->msecToTimeout(currentTime);
        if (_poll_timeout < 0)
          _poll_timeout = msec;
        else if (msec < _poll_timeout)
          _poll_timeout = msec;
      }

      _new_jobs.clear();
    }
  }

  void PollerImpl::run()
  {
    epoll_event events[16];

    time_t pollTime;
    time(&pollTime);
    while (!Tntnet::shouldStop())
    {
      usleep(100);

      append_new_jobs();

      if (_jobs.empty())
        _poll_timeout = -1;

      int ret = ::epoll_wait(_pollFd, events, 16, _poll_timeout);

      if (ret < 0)
      {
        if (errno != EINTR)
          throw cxxtools::SystemError("epoll_wait");
      }
      else if (ret == 0)
      {
        // timeout reached - check for timed out requests and get next timeout

        _poll_timeout = -1;
        time_t currentTime;
        time(&currentTime);
        for (jobs_type::iterator it = _jobs.begin(); it != _jobs.end(); )
        {
          int msec = it->second->msecToTimeout(currentTime);
          if (msec <= 0)
          {
            log_debug("timeout for fd " << it->second->getFd() << " reached");
            jobs_type::iterator it2 = it++;
            _jobs.erase(it2);
          }
          else
          {
            if (_poll_timeout < 0 || msec < _poll_timeout)
              _poll_timeout = msec;
            ++it;
          }
        }
      }
      else
      {
        time_t currentTime;
        time(&currentTime);
        _poll_timeout -= (currentTime - pollTime) * 1000;
        if (_poll_timeout <= 0)
          _poll_timeout = 100;

        pollTime = currentTime;

        // no timeout - process events

        bool rebuildPollFd = false;

        for (int i = 0; i < ret; ++i)
        {
          if (events[i].data.fd == _notify_pipe.getReadFd())
          {
            if (Tntnet::shouldStop())
            {
              log_info("stop poller");
              break;
            }

            char buffer[64];
            _notify_pipe.read(buffer, sizeof(buffer));
          }
          else
          {
            jobs_type::iterator it = _jobs.find(events[i].data.fd);
            if (it == _jobs.end())
            {
              log_fatal("internal error: job for fd " << events[i].data.fd << " not found in jobs-list");
              ::close(events[i].data.fd);
              rebuildPollFd = true;
            }
            else
            {
              Jobqueue::JobPtr j = it->second;
              int ev = events[i].events;
              _jobs.erase(it);

              if (!removeFd(events[i].data.fd))
                rebuildPollFd = true;

              if (ev & EPOLLIN)
                _queue.put(j);
            }
          }
        }

        if (rebuildPollFd)
        {
          // rebuild poll-structure
          log_warn("need to rebuild poll structure");
          close(_pollFd);
          _pollFd = ::epoll_create(256);
          if (_pollFd < 0)
            throw cxxtools::SystemError("epoll_create");

          int ret = fcntl(_notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);
          if (ret < 0)
            throw cxxtools::SystemError("fcntl");

          addFd(_notify_pipe.getReadFd());
          for (jobs_type::iterator it = _jobs.begin(); it != _jobs.end(); ++it)
            addFd(it->first);
        }
      }
    }
  }

#else

  PollerImpl::PollerImpl(Jobqueue& q)
    : _queue(q),
      _poll_timeout(-1)
  {
    fcntl(_notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);

    _pollfds.push_back(pollfd());
    _pollfds.back().fd = _notify_pipe.getReadFd();
    _pollfds.back().events = POLLIN;
    _pollfds.back().revents = 0;
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(_mutex);
    if (!_new_jobs.empty())
    {
      // append new jobs to current
      time_t currentTime;
      time(&currentTime);
      for (jobs_type::iterator it = _new_jobs.begin();
           it != _new_jobs.end(); ++it)
      {
        append(*it);
        int msec = (*it)->msecToTimeout(currentTime);
        if (_poll_timeout < 0 || msec < _poll_timeout)
          _poll_timeout = msec;
      }

      _new_jobs.clear();
    }
  }

  void PollerImpl::append(Jobqueue::JobPtr& job)
  {
    _current_jobs.push_back(job);

    _pollfds.push_back(pollfd());
    _pollfds.back().fd = job->getFd();
    _pollfds.back().events = POLLIN;
  }

  void PollerImpl::run()
  {
    while (!Tntnet::shouldStop())
    {
      usleep(100);
      append_new_jobs();

      try
      {
        ::poll(&_pollfds[0], _pollfds.size(), _poll_timeout);
        _poll_timeout = -1;

        if (_pollfds[0].revents != 0)
        {
          if (Tntnet::shouldStop())
          {
            log_info("stop poller");
            break;
          }

          char buffer[64];
          _notify_pipe.read(buffer, sizeof(buffer));
          _pollfds[0].revents = 0;
        }

        if (_current_jobs.size() > 0)
          dispatch();
      }
      catch (const std::exception& e)
      {
        log_error("error in poll-loop: " << e.what());
      }
    }
  }

  void PollerImpl::doStop()
  {
    _notify_pipe.write('A');
  }

  void PollerImpl::dispatch()
  {
    time_t currentTime;
    time(&currentTime);
    for (unsigned i = 0; i < _current_jobs.size(); )
    {
      if (_pollfds[i + 1].revents & POLLIN)
      {
        // put job into work-queue
        _queue.put(_current_jobs[i]);
        remove(i);
      }
      else if (_pollfds[i + 1].revents != 0)
        remove(i);
      else
      {
        // check timeout
        int msec = _current_jobs[i]->msecToTimeout(currentTime);
        if (msec <= 0)
          remove(i);
        else if (_poll_timeout < 0 || msec < _poll_timeout)
          _poll_timeout = msec;

        ++i;
      }
    }
  }

  void PollerImpl::remove(jobs_type::size_type n)
  {
    // replace job with last job in poller-list
    jobs_type::size_type last = _current_jobs.size() - 1;

    if (n != last)
    {
      _pollfds[n + 1] = _pollfds[last + 1];
      _current_jobs[n] = _current_jobs[last];
    }

    _pollfds.pop_back();
    _current_jobs.pop_back();
  }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    {
      cxxtools::MutexLock lock(_mutex);
      _new_jobs.push_back(job);
    }

    _notify_pipe.write('A');
  }
#endif // #else HAVE_EPOLL

}
