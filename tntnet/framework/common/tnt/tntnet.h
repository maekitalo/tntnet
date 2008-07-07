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

#include <tnt/job.h>
#include <tnt/poller.h>
#include <tnt/dispatcher.h>
#include <tnt/maptarget.h>
#include <tnt/scopemanager.h>
#include <set>

namespace tnt
{
  class ListenerBase;
  class Tntconfig;

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
      static listeners_type allListeners;

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

      /// Returns the minimum number of worker threads.
      unsigned getMinThreads() const          { return minthreads; }
      /// Sets the minimum number of worker threads.
      void setMinThreads(unsigned n);

      /// Returns the maximum number of worker threads.
      unsigned getMaxThreads() const          { return maxthreads; }
      /// Sets the maximum number of worker threads.
      void setMaxThreads(unsigned n)          { maxthreads = n; }

      /// Returns the time in seconds after which cleanup like checking sessiontimeout is done.
      unsigned getTimersleep() const          { return timersleep; }
      /// Sets the time in seconds after which cleanup like checking sessiontimeout is done.
      void setTimersleep(unsigned sec)        { timersleep = sec; }

      /// Returns the time in seconds between thread starts.
      unsigned getThreadStartDelay() const    { return threadstartdelay; }
      /// Sets the time in seconds between thread starts.
      void setThreadStartDelay(unsigned sec)  { threadstartdelay = sec; }

      /// Returns the maximum number of jobs waiting for processing.
      unsigned getQueueSize() const           { return queue.getCapacity(); }
      /// Sets the maximum number of jobs waiting for processing.
      void setQueueSize(unsigned n)           { queue.setCapacity(n); }

      Maptarget& mapUrl(const std::string& url, const std::string& ci)
        { return dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)); }
      void mapUrl(const std::string& url, const std::string& pathinfo, const std::string& ci_)
      {
        Maptarget ci(ci_);
        ci.setPathInfo(pathinfo);
        dispatcher.addUrlMapEntry(std::string(), url, ci);
      }
      Maptarget& mapUrl(const std::string& url, const Maptarget& ci)
        { return dispatcher.addUrlMapEntry(std::string(), url, ci); }
      Maptarget& vMapUrl(const std::string& vhost, const std::string& url, const Maptarget& ci)
        { return dispatcher.addUrlMapEntry(vhost, url, ci); }

      /** Forks a process - returns true in child and false in parent.
       *  A double fork is done to prevent zombies and listener sockets are
       *  closed.
       *
       *  usage:
       *  \code
       *    if (app.forkProcess())
       *    {
       *      // do whatever you need to do in a child process
       *      exit(0);  // this or exec... is mandatory!!!
       *    }
       *  \endcode
       */
      bool forkProcess();

      /** Starts the passed function as a separate process.
       */
      template <typename function>
      void runProcess(function func)
      {
        if (forkProcess())
        {
          func();
          exit(0);
        }
      }
  };

}

#endif // TNT_TNTNET_H

