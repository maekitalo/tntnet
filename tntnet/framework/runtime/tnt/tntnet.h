/* tnt/tntnet.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#ifndef TNT_TNTNET_H
#define TNT_TNTNET_H

#include <cxxtools/arg.h>
#include "tnt/tntconfig.h"
#include "tnt/job.h"
#include "tnt/poller.h"
#include "tnt/dispatcher.h"
#include <set>

namespace tnt
{
  class scope;
  class sessionscope;

  class tntnet
  {
    public:
      typedef std::map<std::string, scope*> scopes_type;
      typedef std::map<std::string, sessionscope*> sessionscopes_type;

    private:
      cxxtools::arg<const char*> conf;
      tntconfig config;
      cxxtools::arg<const char*> propertyfilename;
      cxxtools::arg<bool> debug;

      unsigned minthreads;
      unsigned maxthreads;
      unsigned long threadstartdelay;
      unsigned sessionTimeout;

      jobqueue queue;

      static bool stop;
      typedef std::set<cxxtools::Thread*> listeners_type;
      listeners_type listeners;

      poller pollerthread;
      dispatcher d_dispatcher;

      static std::string pidFileName;

      scopes_type applicationScopes;
      sessionscopes_type sessionScopes;
      cxxtools::Mutex applicationScopesMutex;
      cxxtools::Mutex sessionScopesMutex;

      // helper methods
      void setUser() const;
      void setGroup() const;
      void setDir(const char* def) const;
      int mkDaemon() const;  // returns pipe
      void closeStdHandles() const;

      // noncopyable
      tntnet(const tntnet&);
      tntnet& operator= (const tntnet&);

      void writePidfile(int pid);
      void monitorProcess(int workerPid);
      void initWorkerProcess();
      void workerProcess(int filedes = -1);

      void timerThread();

    public:
      tntnet(int argc, char* argv[]);
      int run();

      static void shutdown();
      static bool shouldStop()   { return stop; }

      jobqueue&   getQueue()                  { return queue; }
      poller&     getPoller()                 { return pollerthread; }
      const dispatcher& getDispatcher() const { return d_dispatcher; }
      const tntconfig&  getConfig() const     { return config; }

      scope* getApplicationScope(const std::string& appname);
      sessionscope* getSessionScope(const std::string& sessioncookie);
      bool hasSessionScope(const std::string& sessioncookie);
      void putSessionScope(const std::string& sessioncookie, sessionscope* s);
      void removeApplicationScope(const std::string& appname);
      void removeSessionScope(const std::string& sessioncookie);
  };

}

#endif // TNT_TNTNET_H

