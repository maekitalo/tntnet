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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

