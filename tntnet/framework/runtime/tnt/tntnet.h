/* tnt/tntnet.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef TNTNET_H
#define TNTNET_H

#include <cxxtools/arg.h>
#include "tnt/tntconfig.h"
#include "tnt/job.h"
#include <set>
#include "tnt/log.h"

namespace tnt
{
  class tntnet
  {
      arg<const char*> arg_ip;
      arg<unsigned short int> arg_port;
      arg<unsigned> arg_numthreads;
      arg<const char*> conf;
      tntconfig config;
      arg<const char*> propertyfilename;
      arg<bool> debug;
      arg<unsigned> arg_lifetime;

      unsigned numthreads;
      unsigned short int port;
      std::string ip;
      unsigned lifetime;

      jobqueue queue;

      static bool stop;
      typedef std::set<Thread*> listeners_type;
      listeners_type listeners;

      static std::string pidFileName;

      // helper methods
      void setUser() const;
      void setGroup() const;
      void mkDaemon() const;
      void closeStdHandles() const;

      // noncopyable
      tntnet(const tntnet&);
      tntnet& operator= (const tntnet&);

      void monitorProcess(int workerPid);
      void workerProcess();

      log_declare_class();

    public:
      tntnet(int argc, char* argv[]);
      int run();

      static void shutdown();
      static bool shouldStop()   { return stop; }
  };

}

#endif // TNTNET_H

