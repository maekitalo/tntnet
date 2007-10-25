/* tnt/tntnet.h
 * Copyright (C) 2003-2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_TNTNET_H
#define TNT_TNTNET_H

#include <cxxtools/arg.h>
#include <tnt/tntconfig.h>
#include <tnt/job.h>
#include <tnt/poller.h>
#include <tnt/dispatcher.h>
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
      unsigned timersleep;

      Jobqueue queue;

      static bool stop;
      typedef std::set<ListenerBase*> listeners_type;
      listeners_type listeners;

      Poller pollerthread;
      Dispatcher dispatcher;

      ScopeManager scopemanager;

      // noncopyable
      Tntnet(const Tntnet&);
      Tntnet& operator= (const Tntnet&);

      void timerTask();

    public:
      Tntnet();

      void init(const Tntconfig& config);
      void listen(const std::string& ipaddr, unsigned short int port);
      void sslListen(const std::string& certificateFile, const std::string& keyFile, const std::string& ipaddr, unsigned short int port);
      void run();

      static void shutdown();
      static bool shouldStop()   { return stop; }

      Jobqueue&   getQueue()                  { return queue; }
      Poller&     getPoller()                 { return pollerthread; }
      const Dispatcher& getDispatcher() const { return dispatcher; }
      ScopeManager& getScopemanager()         { return scopemanager; }

      unsigned getMinThreads() const          { return minthreads; }
      void setMinThreads(unsigned n);

      unsigned getMaxThreads() const          { return maxthreads; }
      void setMaxThreads(unsigned n)          { maxthreads = n; }

      Dispatcher::CompidentType& mapUrl(const std::string& url, const std::string& ci)
        { return dispatcher.addUrlMapEntry(std::string(), url, Dispatcher::CompidentType(ci)); }
      void mapUrl(const std::string& url, const std::string& pathinfo, const std::string& ci_)
      {
        Dispatcher::CompidentType ci(ci_);
        ci.setPathInfo(pathinfo);
        dispatcher.addUrlMapEntry(std::string(), url, ci);
      }
      Dispatcher::CompidentType& mapUrl(const std::string& url, const Dispatcher::CompidentType& ci)
        { return dispatcher.addUrlMapEntry(std::string(), url, ci); }
      Dispatcher::CompidentType& vMapUrl(const std::string& vhost, const std::string& url, const Dispatcher::CompidentType& ci)
        { return dispatcher.addUrlMapEntry(vhost, url, ci); }
  };

}

#endif // TNT_TNTNET_H

