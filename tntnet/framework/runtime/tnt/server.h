/* tnt/server.h
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

#ifndef TNT_SERVER_H
#define TNT_SERVER_H

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

  class server : public cxxtools::Thread
  {
      static cxxtools::Mutex mutex;
      static unsigned nextThreadNumber;

      jobqueue& queue;
      poller& mypoller;

      const dispatcher& ourdispatcher;
      comploader mycomploader;

      unsigned threadNumber;

      typedef std::set<server*> servers_type;
      static servers_type servers;

      static unsigned compLifetime;

      bool processRequest(httpRequest& request, std::iostream& socket, bool keepAlive);

    public:
      server(jobqueue& queue, const dispatcher& dispatcher,
        poller& poller, comploader::load_library_listener* libconfigurator);

      virtual void Run();

      void Dispatch(httpRequest& request, httpReply& reply);
      void cleanup(unsigned seconds)
      { mycomploader.cleanup(seconds); }
      static void addSearchPath(const std::string& path)
      { comploader::addSearchPath(path); }

      static void CleanerThread();
      static void setCompLifetime(unsigned sec)
      { compLifetime = sec; }

    private:
      void executeQuery(const std::string& header);
  };
}

#endif // TNT_SERVER_H

