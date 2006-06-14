/* poller.cpp
 * Copyright (C) 2005 Tommi Maekitalo
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
#include <cxxtools/log.h>
#include <unistd.h>
#include <fcntl.h>

log_define("tntnet.poller")

namespace tnt
{
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
        ::poll(pollfds.data(), current_jobs.size() + 1, poll_timeout);
        if (Tntnet::shouldStop())
        {
          log_warn("stop poller");
          break;
        }

        poll_timeout = -1;

        if (pollfds[0].revents != 0)
        {
          log_debug("read notify-pipe");
          char ch;
          ::read(notify_pipe[0], &ch, 1);
          pollfds[0].revents = 0;
        }

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
    ::write(notify_pipe[1], &ch, 1);
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
    ::write(notify_pipe[1], &ch, 1);

    log_debug("addIdleJob ready");
  }

}
