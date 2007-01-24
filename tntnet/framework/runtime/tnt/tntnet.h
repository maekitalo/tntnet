/* tnt/tntnet.h
 * Copyright (C) 2003-2007 Tommi Maekitalo
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

#ifndef TNT_TNTNET_H
#define TNT_TNTNET_H

#include <cxxtools/arg.h>
#include "tnt/tntconfig.h"
#include "tnt/job.h"
#include "tnt/poller.h"
#include "tnt/dispatcher.h"
#include <tnt/scopemanager.h>
#include <set>

namespace tnt
{
  class ListenerBase;

  class Tntnet
  {
      unsigned minthreads;
      unsigned maxthreads;
      unsigned long threadstartdelay;

      Jobqueue queue;

      static bool stop;
      typedef std::set<ListenerBase*> listeners_type;
      listeners_type listeners;

      Poller pollerthread;
      Dispatcher d_dispatcher;

      ScopeManager scopemanager;

      // noncopyable
      Tntnet(const Tntnet&);
      Tntnet& operator= (const Tntnet&);

      void timerTask();

    public:
      Tntnet()
        : pollerthread(queue)
        { }

      void init(const Tntconfig& config);
      void run();

      static void shutdown();
      static bool shouldStop()   { return stop; }

      Jobqueue&   getQueue()                  { return queue; }
      Poller&     getPoller()                 { return pollerthread; }
      const Dispatcher& getDispatcher() const { return d_dispatcher; }
      ScopeManager& getScopemanager()         { return scopemanager; }
  };

}

#endif // TNT_TNTNET_H

