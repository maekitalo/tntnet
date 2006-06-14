/* tnt/tntnet.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
      std::string configFile;
      Tntconfig config;
      cxxtools::Arg<const char*> propertyfilename;
      cxxtools::Arg<bool> debug;
      bool isDaemon;

      unsigned minthreads;
      unsigned maxthreads;
      unsigned long threadstartdelay;

      Jobqueue queue;

      static bool stop;
      static int ret;
      typedef std::set<ListenerBase*> listeners_type;
      listeners_type listeners;

      Poller pollerthread;
      Dispatcher d_dispatcher;

      static std::string pidFileName;

      ScopeManager scopemanager;

      // helper methods
      void setUser() const;
      void setGroup() const;
      void setDir(const char* def) const;
      int mkDaemon() const;  // returns pipe
      void closeStdHandles() const;

      // noncopyable
      Tntnet(const Tntnet&);
      Tntnet& operator= (const Tntnet&);

      void initLogging();
      void writePidfile(int pid);
      void monitorProcess(int workerPid);
      void initWorkerProcess();
      void workerProcess(int filedes = -1);

      void timerTask();
      void loadConfiguration();

    public:
      Tntnet(int& argc, char* argv[]);
      int run();

      static void shutdown();
      static void restart();
      static bool shouldStop()   { return stop; }

      Jobqueue&   getQueue()                  { return queue; }
      Poller&     getPoller()                 { return pollerthread; }
      const Dispatcher& getDispatcher() const { return d_dispatcher; }
      const Tntconfig&  getConfig() const     { return config; }
      ScopeManager& getScopemanager()         { return scopemanager; }
  };

}

#endif // TNT_TNTNET_H

