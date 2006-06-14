/* tnt/poller.h
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

#ifndef TNT_POLLER_H
#define TNT_POLLER_H

#include "tnt/job.h"
#include <cxxtools/dynbuffer.h>
#include <cxxtools/thread.h>
#include <sys/poll.h>

namespace tnt
{
  class Poller : public cxxtools::AttachedThread
  {
      Jobqueue& queue;
      int notify_pipe[2];

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

    public:
      Poller(Jobqueue& q);

      virtual void run();
      void doStop();
      void addIdleJob(Jobqueue::JobPtr job);
  };

}

#endif // TNT_POLLER_H

