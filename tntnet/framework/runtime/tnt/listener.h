/* tnt/listener.h
 * Copyright (C) 2003 Tommi Maekitalo
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

#ifndef TNT_LISTENER_H
#define TNT_LISTENER_H

#include <config.h>
#include <cxxtools/thread.h>
#include "tnt/job.h"

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

