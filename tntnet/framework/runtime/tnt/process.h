/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNT_PROCESS_H
#define TNT_PROCESS_H

#include <cxxtools/pipe.h>
#include <string>

namespace tnt
{
  class Process
  {
      bool daemon;
      std::string dir;
      std::string rootdir;
      std::string user;
      std::string group;
      std::string pidfile;

      bool exitRestart;

      int mkDaemon(cxxtools::Pipe& pipe);
      void runMonitor(cxxtools::Pipe& mainPipe);
      void initWorker();

    protected:
      void setDaemon(bool sw = true)        { daemon = sw; }
      void setDir(const std::string& v)     { dir = v; }
      void setRootdir(const std::string& v) { rootdir = v; }
      void setUser(const std::string& v)    { user = v; }
      void setGroup(const std::string& v)   { group = v; }
      void setPidFile(const std::string& v) { pidfile = v; }

    public:
      explicit Process(bool daemon_ = false);
      ~Process();

      void run();
      void shutdown();
      void restart();

    protected:
      virtual void onInit() = 0;
      virtual void doWork() = 0;
      virtual void doShutdown() = 0;
  };
}

#endif // TNT_PROCESS_H
