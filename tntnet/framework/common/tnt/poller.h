/* tnt/poller.h
 * Copyright (C) 2005-2006 Tommi Maekitalo
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


#ifndef TNT_POLLER_H
#define TNT_POLLER_H

#include <tnt/job.h>
#include <cxxtools/thread.h>
#include <cxxtools/noncopyable.h>

namespace tnt
{
  class PollerIf : private cxxtools::NonCopyable
  {
    public:
      virtual ~PollerIf();
      virtual void run() = 0;
      virtual void doStop() = 0;
      virtual void addIdleJob(Jobqueue::JobPtr job) = 0;
  };

  class Poller
  {
      PollerIf* impl;

    public:
      explicit Poller(Jobqueue& q);
      ~Poller()
        { delete impl; }

      void run();
      void doStop()                          { impl->doStop(); }
      void addIdleJob(Jobqueue::JobPtr job)  { impl->addIdleJob(job); }
  };

}

#endif // TNT_POLLER_H

