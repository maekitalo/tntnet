/*
 * Copyright (C) 2003-2007 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef TNT_TNTNET_H
#define TNT_TNTNET_H

#include <tnt/job.h>
#include <tnt/poller.h>
#include <tnt/dispatcher.h>
#include <tnt/maptarget.h>
#include <tnt/scopemanager.h>
#include <cxxtools/condition.h>
#include <cxxtools/mutex.h>
#include <set>

namespace tnt
{
  class ListenerBase;
  class Tntconfig;

  /**
   @brief Main application class for stand alone webapplication.

   This is the class to use to run a webapplication as a stand alone
   application. Using this a application runs itself as a webserver instead of
   using a tntnet application server.

   To create a application you need to:
     * instantiate a Tntnet class
     * call listen and/or sslListen for each ip address to listen on
     * set up your url mappings using mapUrl or vMapUrl
     * call run()
   */
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

      cxxtools::AttachedThread pollerthread;
      Poller poller;
      Dispatcher dispatcher;

      ScopeManager scopemanager;

      // noncopyable
      Tntnet(const Tntnet&);
      Tntnet& operator= (const Tntnet&);

      void timerTask();

      static cxxtools::Condition timerStopCondition;
      static cxxtools::Mutex timeStopMutex;

    public:
      /// Initialize a default Tntnet-application without mappings.
      Tntnet();

      /// Read settings from a Tntconfig object.
      void init(const Tntconfig& config);
      /// Set up a listener on the specified ip address and port.
      /// Listening on the port does not actually happen here but when the
      /// application is started using the run-method.
      void listen(const std::string& ipaddr, unsigned short int port);
      /// Set up a ssl listener on the specified ip address and port.
      /// Listening on the port does not actually happen here but when the
      /// application is started using the run-method.
      void sslListen(const std::string& certificateFile, const std::string& keyFile, const std::string& ipaddr, unsigned short int port);
      /// Starts all needed threads and the application loop.
      /// If no listeners are set up using listen or sslListen, a default
      /// listener is instantiated. By default it listens on the ip address
      /// 0.0.0.0 (i.e. all addresses). The port 80 is used, when started as
      /// root user, port 8000 otherwise.
      void run();

      /// Shut down all instances of tntnet servers.
      static void shutdown();
      /// Tells, if a shutdown request was initiated.
      static bool shouldStop()   { return stop; }

      /// Returns the queue, which helds active http requests.
      Jobqueue&   getQueue()                  { return queue; }
      /// Returns a reference to the poller thread object.
      Poller&     getPoller()                 { return poller; }
      /// Returns a reference to the dispatcher object.
      const Dispatcher& getDispatcher() const { return dispatcher; }
      /// Returns a reference to the scope manager object.
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

      /// Adds a mapping from a url to a component.
      /// The url is specified using a regular expression and the mapped target
      /// may contain back references to the expression using $1, $2, ... .
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

