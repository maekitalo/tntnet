/* tnt/worker.h
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

#ifndef TNT_WORKER_H
#define TNT_WORKER_H

#include <string>
#include <cxxtools/tcpstream.h>
#include <cxxtools/thread.h>
#include <tnt/comploader.h>

namespace tnt
{
  class httpRequest;
  class httpReply;
  class dispatcher;
  class jobqueue;
  class poller;

  class worker : public cxxtools::Thread
  {
      static cxxtools::Mutex mutex;
      static unsigned nextThreadNumber;

      jobqueue& queue;
      poller& mypoller;

      const dispatcher& ourdispatcher;
      comploader mycomploader;

      unsigned threadNumber;

      typedef std::set<worker*> workers_type;
      static workers_type workers;

      static unsigned compLifetime;
      static unsigned minThreads;

      unsigned processRequest(httpRequest& request, std::iostream& socket, unsigned keepAliveCount);

    public:
      worker(jobqueue& queue, const dispatcher& dispatcher,
        poller& poller, const tntconfig& config);
      ~worker();

      virtual void Run();

      void Dispatch(httpRequest& request, httpReply& reply, unsigned keepAliveCount = 0);
      void cleanup(unsigned seconds)
      { mycomploader.cleanup(seconds); }
      static void addSearchPath(const std::string& path)
      { comploader::addSearchPath(path); }

      static void CleanerThread();
      static void setCompLifetime(unsigned sec)
      { compLifetime = sec; }

      static workers_type::size_type getCountThreads();
      static void setMinThreads(unsigned n)
      { minThreads = n; }
  };
}

#endif // TNT_WORKER_H

