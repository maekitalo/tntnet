/* tnt/listener.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef TNT_LISTENER_H
#define TNT_LISTENER_H

#include <config.h>
#include <cxxtools/thread.h>
#include "tnt/job.h"

#ifdef USE_SSL
#  include "tnt/ssl.h"
#endif

namespace tnt
{
  class ListenerBase : public cxxtools::AttachedThread
  {
      std::string ipaddr;
      unsigned short int port;

    public:
      ListenerBase(const std::string& ipaddr_, unsigned short int port_)
        : ipaddr(ipaddr_),
          port(port_)
          { }

      void doStop();
  };

  class Listener : public ListenerBase
  {
      cxxtools::net::Server server;
      Jobqueue& queue;
      static int backlog;
      static unsigned listenRetry;

    public:
      Listener(const std::string& ipaddr, unsigned short int port, Jobqueue& q);
      virtual void run();

      static void setBacklog(int backlog_)   { backlog = backlog_; }
      static int getBacklog()                { return backlog; }
      static void setListenRetry(unsigned listenRetry_)   { listenRetry = listenRetry_; }
      static unsigned getListenRetry()                { return listenRetry; }
  };

#ifdef USE_SSL
  class Ssllistener : public ListenerBase
  {
      SslServer server;
      Jobqueue& queue;

    public:
      Ssllistener(const char* certificateFile, const char* keyFile,
          const std::string& ipaddr, unsigned short int port, Jobqueue& q);
      virtual void run();
  };
#endif // USE_SSL

}

#endif // TNT_LISTENER_H

