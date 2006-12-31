/* poller.cpp
 * Copyright (C) 2005-2006 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "tnt/poller.h"
#include "tnt/tntnet.h"
#include "tnt/syserror.h"
#include <cxxtools/log.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#include <errno.h>
#endif

log_define("tntnet.poller")

namespace tnt
{
#ifdef HAVE_EPOLL

  Poller::Poller(Jobqueue& q)
    : queue(q),
      pollFd(-1)
  {
    pollFd = ::epoll_create(256);
    if (pollFd < 0)
      throw SysError("epoll_create");

    pipe(notify_pipe);
    fcntl(notify_pipe[0], F_SETFL, O_NONBLOCK);
    addFd(notify_pipe[0], EPOLLIN);
  }

  void Poller::addFd(int fd, uint32_t event)
  {
    log_debug("addFd(" << fd << ')');

    epoll_event e;
    e.events = event;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_ADD, fd, &e);
    if (ret < 0)
      throw SysError("epoll_ctl(EPOLL_CTL_ADD)");
  }

  void Poller::removeFd(int fd)
  {
    log_debug("removeFd(" << fd << ')');

    epoll_event e;
    e.data.fd = fd;
    int ret = ::epoll_ctl(pollFd, EPOLL_CTL_DEL, fd, &e);
    if (ret < 0)
    {
      if (errno == EBADF)
        log_debug("fd " << fd << " couldn't be removed");
      else
        throw SysError("epoll_ctl(EPOLL_CTL_DEL)");
    }
  }

  void Poller::doStop()
  {
    log_debug("notify stop");
    char ch = 'A';
    int ret = ::write(notify_pipe[1], &ch, 1);
    if (ret < 0)
      throw SysError("write");
  }

  void Poller::addIdleJob(Jobqueue::JobPtr job)
  {
    log_debug("addIdleJob " << job->getFd());

    {
      cxxtools::MutexLock lock(mutex);
      new_jobs.insert(job);

      if (new_jobs.size() <= 1)
      {
        char ch = 'A';
        int ret = ::write(notify_pipe[1], &ch, 1);
        if (ret < 0)
          throw SysError("write");
      }
    }

    log_debug("addIdleJob ready");
  }

  void Poller::append_new_jobs()
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
        addFd((*it)->getFd(), EPOLLIN);
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

  void Poller::run()
  {
    epoll_event events[16];

    time_t pollTime;
    time(&pollTime);
    while (!Tntnet::shouldStop())
    {
      append_new_jobs();

      try
      {
        if (jobs.size() == 0)
          poll_timeout = -1;

        log_debug("epoll_wait with timeout " << poll_timeout << " ms");
        usleep(100);
        int ret = ::epoll_wait(pollFd, events, 16, poll_timeout);
        if (ret < 0)
          throw SysError("epoll_wait");
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
          for (int i = 0; i < ret; ++i)
          {
            if (events[i].data.fd == notify_pipe[0])
            {
              log_debug("read notify-pipe");
              char buffer[64];
              ssize_t n = ::read(notify_pipe[0], &buffer, sizeof(buffer));
              log_debug("read returns " << n);

              if (Tntnet::shouldStop())
              {
                log_warn("stop poller");
                break;
              }
            }
            else
            {
              jobs_type::iterator it = jobs.find(events[i].data.fd);
              if (it == jobs.end())
              {
                log_fatal("internal error: job for fd " << events[i].data.fd << " not found in jobs-list");
                removeFd(events[i].data.fd);
                ::close(events[i].data.fd);
                throw std::runtime_error("job not found in jobs-list");
              }

              if ((events[i].events & (EPOLLERR | EPOLLHUP)) != 0)
              {
                log_debug("remove fd " << it->first << " from queue");
              }
              else
              {
                log_debug("put fd " << it->first << " back in queue");
                queue.put(it->second);
              }

              jobs.erase(events[i].data.fd);
              removeFd(events[i].data.fd);
            }
          }
        }
      }
      catch (const std::exception& e)
      {
        log_warn("error in poll-loop: " << e.what());
        poll_timeout = 100;
      }
    }
  }

#else

  Poller::Poller(Jobqueue& q)
    : queue(q),
      poll_timeout(-1)
  {
    pipe(notify_pipe);
    fcntl(notify_pipe[0], F_SETFL, O_NONBLOCK);

    pollfds.reserve(16);
    pollfds[0].fd = notify_pipe[0];
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;
  }

  void Poller::append_new_jobs()
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

  void Poller::append(Jobqueue::JobPtr& job)
  {
    current_jobs.push_back(job);

    pollfd& p = *(pollfds.data() + current_jobs.size());
    p.fd = job->getFd();
    p.events = POLLIN;
  }

  void Poller::run()
  {
    while (!Tntnet::shouldStop())
    {
      append_new_jobs();

      try
      {
        log_debug("poll timeout=" << poll_timeout);
        usleep(100);
        ::poll(pollfds.data(), current_jobs.size() + 1, poll_timeout);
        poll_timeout = -1;

        if (pollfds[0].revents != 0)
        {
          log_debug("read notify-pipe");
          char buffer[64];
          ssize_t n = ::read(notify_pipe[0], &buffer, sizeof(buffer));
          log_debug("read returns " << n);
          pollfds[0].revents = 0;

          if (Tntnet::shouldStop())
          {
            log_warn("stop poller");
            break;
          }
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

  void Poller::doStop()
  {
    log_debug("notify stop");
    char ch = 'A';
    int ret = ::write(notify_pipe[1], &ch, 1);
    if (ret < 0)
      throw SysError("write");
  }

  void Poller::dispatch()
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

  void Poller::remove(jobs_type::size_type n)
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

  void Poller::addIdleJob(Jobqueue::JobPtr job)
  {
    log_debug("addIdleJob " << job->getFd());

    {
      cxxtools::MutexLock lock(mutex);
      new_jobs.push_back(job);
    }

    log_debug("notify " << job->getFd());

    char ch = 'A';
    int ret = ::write(notify_pipe[1], &ch, 1);
    if (ret < 0)
      throw SysError("write");

    log_debug("addIdleJob ready");
  }
#endif // #else HAVE_EPOLL

}
