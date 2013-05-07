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
  PollerIf::~PollerIf()
  { }

  Poller::Poller(Jobqueue& q)
    : impl(new PollerImpl(q))
  { }

  void Poller::run()
  {
    impl->run();
  }

#ifdef WITH_EPOLL

  PollerImpl::PollerImpl(Jobqueue& q)
    : queue(q),
      pollFd(-1)
  {
    pollFd = ::epoll_create(256);
    if (pollFd < 0)
      throw cxxtools::SystemError("epoll_create");

    fcntl(notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);
    addFd(notify_pipe.getReadFd());
  }

  PollerImpl::~PollerImpl()
  {
    close(pollFd);
  }

  void PollerImpl::addFd(int fd)
  {
    epoll_event e;
    e.events = EPOLLIN;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_ADD, fd, &e);
    if (ret < 0)
      throw cxxtools::SystemError("epoll_ctl(EPOLL_CTL_ADD)");
  }

  bool PollerImpl::removeFd(int fd)
  {
    epoll_event e;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_DEL, fd, &e);
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
  {
    notify_pipe.write('A');
  }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    cxxtools::MutexLock lock(mutex);
    new_jobs.push_back(job);
    notify_pipe.write('A');
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(mutex);
    if (!new_jobs.empty())
    {
      // append new jobs to current
      time_t currentTime;
      time(&currentTime);
      for (new_jobs_type::iterator it = new_jobs.begin();
           it != new_jobs.end(); ++it)
      {
        addFd((*it)->getFd());
        jobs[(*it)->getFd()] = *it;

        int msec = (*it)->msecToTimeout(currentTime);
        if (poll_timeout < 0)
          poll_timeout = msec;
        else if (msec < poll_timeout)
          poll_timeout = msec;
      }

      new_jobs.clear();
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

      if (jobs.size() == 0)
        poll_timeout = -1;

      int ret = ::epoll_wait(pollFd, events, 16, poll_timeout);

      if (ret < 0)
      {
        if (errno != EINTR)
          throw cxxtools::SystemError("epoll_wait");
      }
      else if (ret == 0)
      {
        // timeout reached - check for timed out requests and get next timeout

        poll_timeout = -1;
        time_t currentTime;
        time(&currentTime);
        for (jobs_type::iterator it = jobs.begin(); it != jobs.end(); )
        {
          int msec = it->second->msecToTimeout(currentTime);
          if (msec <= 0)
          {
            log_debug("timeout for fd " << it->second->getFd() << " reached");
            jobs_type::iterator it2 = it++;
            jobs.erase(it2);
          }
          else
          {
            if (poll_timeout < 0 || msec < poll_timeout)
              poll_timeout = msec;
            ++it;
          }
        }
      }
      else
      {
        time_t currentTime;
        time(&currentTime);
        poll_timeout -= (currentTime - pollTime) * 1000;
        if (poll_timeout <= 0)
          poll_timeout = 100;

        pollTime = currentTime;

        // no timeout - process events

        bool rebuildPollFd = false;

        for (int i = 0; i < ret; ++i)
        {
          if (events[i].data.fd == notify_pipe.getReadFd())
          {
            if (Tntnet::shouldStop())
            {
              log_info("stop poller");
              break;
            }

            char buffer[64];
            notify_pipe.read(buffer, sizeof(buffer));
          }
          else
          {
            jobs_type::iterator it = jobs.find(events[i].data.fd);
            if (it == jobs.end())
            {
              log_fatal("internal error: job for fd " << events[i].data.fd << " not found in jobs-list");
              ::close(events[i].data.fd);
              rebuildPollFd = true;
            }
            else
            {
              Jobqueue::JobPtr j = it->second;
              int ev = events[i].events;  
              jobs.erase(it);

              if (!removeFd(events[i].data.fd))
                rebuildPollFd = true;

              if (ev & EPOLLIN)
                queue.put(j);
            }
          }
        }

        if (rebuildPollFd)
        {
          // rebuild poll-structure
          log_warn("need to rebuild poll structure");
          close(pollFd);
          pollFd = ::epoll_create(256);
          if (pollFd < 0)
            throw cxxtools::SystemError("epoll_create");

          int ret = fcntl(notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);
          if (ret < 0)
            throw cxxtools::SystemError("fcntl");

          addFd(notify_pipe.getReadFd());
          for (jobs_type::iterator it = jobs.begin(); it != jobs.end(); ++it)
            addFd(it->first);
        }
      }
    }
  }

#else

  PollerImpl::PollerImpl(Jobqueue& q)
    : queue(q),
      poll_timeout(-1)
  {
    fcntl(notify_pipe.getReadFd(), F_SETFL, O_NONBLOCK);

    pollfds.push_back(pollfd());
    pollfds.back().fd = notify_pipe.getReadFd();
    pollfds.back().events = POLLIN;
    pollfds.back().revents = 0;
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(mutex);
    if (!new_jobs.empty())
    {
      // append new jobs to current
      time_t currentTime;
      time(&currentTime);
      for (jobs_type::iterator it = new_jobs.begin();
           it != new_jobs.end(); ++it)
      {
        append(*it);
        int msec = (*it)->msecToTimeout(currentTime);
        if (poll_timeout < 0 || msec < poll_timeout)
          poll_timeout = msec;
      }

      new_jobs.clear();
    }
  }

  void PollerImpl::append(Jobqueue::JobPtr& job)
  {
    current_jobs.push_back(job);

    pollfds.push_back(pollfd());
    pollfds.back().fd = job->getFd();
    pollfds.back().events = POLLIN;
  }

  void PollerImpl::run()
  {
    while (!Tntnet::shouldStop())
    {
      usleep(100);
      append_new_jobs();

      try
      {
        ::poll(&pollfds[0], pollfds.size(), poll_timeout);
        poll_timeout = -1;

        if (pollfds[0].revents != 0)
        {
          if (Tntnet::shouldStop())
          {
            log_info("stop poller");
            break;
          }

          char buffer[64];
          notify_pipe.read(buffer, sizeof(buffer));
          pollfds[0].revents = 0;
        }

        if (current_jobs.size() > 0)
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
    notify_pipe.write('A');
  }

  void PollerImpl::dispatch()
  {
    time_t currentTime;
    time(&currentTime);
    for (unsigned i = 0; i < current_jobs.size(); )
    {
      if (pollfds[i + 1].revents & POLLIN)
      {
        // put job into work-queue
        queue.put(current_jobs[i]);
        remove(i);
      }
      else if (pollfds[i + 1].revents != 0)
        remove(i);
      else
      {
        // check timeout
        int msec = current_jobs[i]->msecToTimeout(currentTime);
        if (msec <= 0)
          remove(i);
        else if (poll_timeout < 0 || msec < poll_timeout)
          poll_timeout = msec;

        ++i;
      }
    }
  }

  void PollerImpl::remove(jobs_type::size_type n)
  {
    // replace job with last job in poller-list
    jobs_type::size_type last = current_jobs.size() - 1;

    if (n != last)
    {
      pollfds[n + 1] = pollfds[last + 1];
      current_jobs[n] = current_jobs[last];
    }

    pollfds.pop_back();
    current_jobs.pop_back();
  }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    {
      cxxtools::MutexLock lock(mutex);
      new_jobs.push_back(job);
    }

    notify_pipe.write('A');
  }
#endif // #else HAVE_EPOLL

}
