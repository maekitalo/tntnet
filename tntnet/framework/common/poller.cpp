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
#include <cxxtools/syserror.h>
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
    log_debug("addFd(" << fd << ')');

    epoll_event e;
    e.events = EPOLLIN;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_ADD, fd, &e);
    if (ret < 0)
      throw cxxtools::SystemError("epoll_ctl(EPOLL_CTL_ADD)");
  }

  bool PollerImpl::removeFd(int fd)
  {
    log_debug("removeFd(" << fd << ')');

    epoll_event e;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_DEL, fd, &e);
    if (ret < 0)
    {
      if (errno == EBADF || errno == ENOENT)
      {
        log_debug("fd " << fd << " couldn't be removed");
        return false;
      }
      else
        throw cxxtools::SystemError("epoll_ctl(EPOLL_CTL_DEL)");
    }

    return true;
  }

  void PollerImpl::doStop()
  {
    log_debug("notify stop");
    notify_pipe.write('A');
  }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    log_debug("addIdleJob " << job->getFd());

    {
      cxxtools::MutexLock lock(mutex);
      new_jobs.insert(job);
      notify_pipe.write('A');
    }

    log_debug("addIdleJob ready");
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(mutex);
    if (!new_jobs.empty())
    {
      // append new jobs to current
      log_debug("add " << new_jobs.size() << " new jobs to poll-list");

      time_t currentTime;
      time(&currentTime);
      for (new_jobs_type::iterator it = new_jobs.begin();
           it != new_jobs.end(); ++it)
      {
        addFd((*it)->getFd());
        jobs[(*it)->getFd()] = *it;

        int msec;
        if (poll_timeout < 0)
          poll_timeout = (*it)->msecToTimeout(currentTime);
        else if ((msec = (*it)->msecToTimeout(currentTime)) < poll_timeout)
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

      log_debug("epoll_wait with timeout " << poll_timeout << " ms");
      int ret = ::epoll_wait(pollFd, events, 16, poll_timeout);

      if (ret < 0)
      {
        if (errno != EINTR)
          throw cxxtools::SystemError("epoll_wait");
      }
      else if (ret == 0)
      {
        // timeout reached - check for timed out requests and get next timeout

        log_debug("timeout reached");

        poll_timeout = -1;
        time_t currentTime;
        time(&currentTime);
        for (jobs_type::iterator it = jobs.begin(); it != jobs.end(); )
        {
          int msec = it->second->msecToTimeout(currentTime);
          if (msec <= 0)
          {
            log_debug("keep-alive-timeout reached");
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
        log_debug(ret << " events occured");

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

            log_debug("read notify-pipe");
            char buffer[64];
            ssize_t n = notify_pipe.read(buffer, sizeof(buffer));
            log_debug("read returns " << n);
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
              {
                log_debug("put fd " << it->first << " back in queue");
                queue.put(j);
              }
              else
              {
                log_debug("remove fd " << it->first << " from queue");
              }
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

    pollfds.reserve(16);
    pollfds[0].fd = notify_pipe.getReadFd();
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
  }

  void PollerImpl::append_new_jobs()
  {
    cxxtools::MutexLock lock(mutex);
    if (!new_jobs.empty())
    {
      // append new jobs to current
      log_debug("add " << new_jobs.size() << " new jobs to poll-list");

      pollfds.reserve(current_jobs.size() + new_jobs.size() + 1);

      time_t currentTime;
      time(&currentTime);
      for (jobs_type::iterator it = new_jobs.begin();
           it != new_jobs.end(); ++it)
      {
        append(*it);
        int msec;
        if (poll_timeout < 0)
          poll_timeout = (*it)->msecToTimeout(currentTime);
        else if ((msec = (*it)->msecToTimeout(currentTime)) < poll_timeout)
          poll_timeout = msec;
      }

      new_jobs.clear();
    }
  }

  void PollerImpl::append(Jobqueue::JobPtr& job)
  {
    current_jobs.push_back(job);

    pollfd& p = *(pollfds.data() + current_jobs.size());
    p.fd = job->getFd();
    p.events = POLLIN;
  }

  void PollerImpl::run()
  {
    while (!Tntnet::shouldStop())
    {
      usleep(100);
      append_new_jobs();

      try
      {
        log_debug("poll timeout=" << poll_timeout);
        ::poll(pollfds.data(), current_jobs.size() + 1, poll_timeout);
        poll_timeout = -1;

        if (pollfds[0].revents != 0)
        {
          if (Tntnet::shouldStop())
          {
            log_info("stop poller");
            break;
          }

          log_debug("read notify-pipe");
          char buffer[64];
          ssize_t n = notify_pipe.read(&buffer, sizeof(buffer));
          log_debug("read returns " << n);
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
    log_debug("notify stop");
    notify_pipe.write('A');
  }

  void PollerImpl::dispatch()
  {
    log_debug("dispatch " << current_jobs.size() << " jobs");

    time_t currentTime;
    time(&currentTime);
    for (unsigned i = 0; i < current_jobs.size(); )
    {
      if (pollfds[i + 1].revents & POLLIN)
      {
        log_debug("job found " << pollfds[i + 1].fd);

        // put job into work-queue
        queue.put(current_jobs[i]);
        remove(i);
      }
      else if (pollfds[i + 1].revents != 0)
      {
        log_debug("pollevent " << std::hex << pollfds[i + 1].revents << " on fd "  << pollfds[i + 1].fd);
        remove(i);
      }
      else
      {
        // check timeout
        int msec = current_jobs[i]->msecToTimeout(currentTime);
        if (msec <= 0)
        {
          log_debug("keep-alive-timeout reached");
          remove(i);
        }
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

    current_jobs.pop_back();
  }

  void PollerImpl::addIdleJob(Jobqueue::JobPtr job)
  {
    log_debug("addIdleJob " << job->getFd());

    {
      cxxtools::MutexLock lock(mutex);
      new_jobs.push_back(job);
    }

    log_debug("notify " << job->getFd());

    notify_pipe.write('A');

    log_debug("addIdleJob ready");
  }
#endif // #else HAVE_EPOLL

}
