/* tnt/pollerimpl.h
 * Copyright (C) 2007 Tommi Maekitalo
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


#ifndef TNT_POLLERIMPL_H
#define TNT_POLLERIMPL_H

#include <config.h>
#include "tnt/job.h"
#include <cxxtools/thread.h>
#include <cxxtools/pipe.h>

#ifdef WITH_EPOLL
#  include <map>
#  include <set>
#else
#  include <cxxtools/dynbuffer.h>
#  include <sys/poll.h>
#endif

namespace tnt
{
  class PollerImpl : public PollerIf
  {
      Jobqueue& queue;

      cxxtools::Pipe notify_pipe;

#ifdef WITH_EPOLL

      int pollFd;

      typedef std::map<int, Jobqueue::JobPtr> jobs_type;
      typedef std::set<Jobqueue::JobPtr> new_jobs_type;
      jobs_type jobs;
      new_jobs_type new_jobs;
      int poll_timeout;

      cxxtools::Mutex mutex;

      void addFd(int fd);
      void removeFd(int fd);
      void append_new_jobs();

#else

      typedef std::deque<Jobqueue::JobPtr> jobs_type;

      jobs_type current_jobs;
      cxxtools::Dynbuffer<pollfd> pollfds;

      jobs_type new_jobs;

      cxxtools::Mutex mutex;
      int poll_timeout;

      void append_new_jobs();
      void append(Jobqueue::JobPtr& job);
      void dispatch();
      void remove(jobs_type::size_type n);

#endif // #else WITH_EPOLL

    public:
      PollerImpl(Jobqueue& q);
#ifdef WITH_EPOLL
      ~PollerImpl();
#endif

      virtual void run();
      void doStop();
      void addIdleJob(Jobqueue::JobPtr job);
  };

}

#endif // TNT_POLLER_H

