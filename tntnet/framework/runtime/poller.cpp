/* poller.cpp
   Copyright (C) 2005 Tommi Maekitalo

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

#include "tnt/poller.h"
#include "tnt/tntnet.h"
#include <cxxtools/log.h>
#include <unistd.h>
#include <fcntl.h>

log_define("tntnet.poll");

namespace tnt
{
  poller::poller(jobqueue& q)
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

  void poller::append_new_jobs()
  {
    cxxtools::MutexLock lock(mutex);
    if (!new_jobs.empty())
    {
      // append new jobs to current
      log_debug("add " << new_jobs.size() << " new jobs to poll-list");

      pollfds.reserve(current_jobs.size() + new_jobs.size() + 1);

      for (jobs_type::iterator it = new_jobs.begin();
           it != new_jobs.end(); ++it)
      {
        append(*it);
        int msec;
        if (poll_timeout < 0)
          poll_timeout = (*it)->msecToTimeout();
        else if ((msec = (*it)->msecToTimeout()) < poll_timeout)
          poll_timeout = msec;
      }

      new_jobs.clear();
    }
  }

  void poller::append(jobqueue::job_ptr& job)
  {
    current_jobs.push_back(job);

    pollfd& p = *(pollfds.data() + current_jobs.size());
    p.fd = job->getFd();
    p.events = POLLIN;
  }

  void poller::Run()
  {
    while (!tntnet::shouldStop())
    {
      append_new_jobs();

      try
      {
        log_debug("poll timeout=" << poll_timeout);
        int ret = ::poll(pollfds.data(), current_jobs.size() + 1,
          poll_timeout);
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

  void poller::dispatch()
  {
    log_debug("dispatch " << current_jobs.size() << " jobs");

    for (unsigned i = 0; i < current_jobs.size(); )
    {
      if (pollfds[i + 1].revents != 0)
      {
        log_debug("job found " << pollfds[i + 1].fd);

        // put job into work-queue
        queue.put(current_jobs[i]);
        remove(i);
      }
      else
      {
        // check timeout
        int msec = current_jobs[i]->msecToTimeout();
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

  void poller::remove(int i)
  {
    // replace job with last job in poller-list
    jobs_type::size_type last = current_jobs.size() - 1;

    pollfds[i + 1] = pollfds[last + 1];

    current_jobs[i] = current_jobs[last];
    current_jobs.pop_back();
  }

  void poller::addIdleJob(jobqueue::job_ptr job)
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
