/* tnt/poller.h
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

#ifndef TNT_POLLER_H
#define TNT_POLLER_H

#include "tnt/job.h"
#include <cxxtools/dynbuffer.h>
#include <cxxtools/thread.h>
#include <sys/poll.h>

namespace tnt
{
  class poller : public cxxtools::Thread
  {
      jobqueue& queue;
      int notify_pipe[2];

      typedef std::deque<jobqueue::job_ptr> jobs_type;

      jobs_type current_jobs;
      cxxtools::dynbuffer<pollfd> pollfds;

      jobs_type new_jobs;

      cxxtools::Mutex mutex;
      int poll_timeout;

      void append_new_jobs();
      void append(jobqueue::job_ptr& job);
      void dispatch();
      void remove(int i);

    public:
      poller(jobqueue& q);

      virtual void Run();
      void addIdleJob(jobqueue::job_ptr job);
  };

}

#endif // TNT_POLLER_H

