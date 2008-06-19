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
#include <tnt/maptarget.h>
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
      unsigned getTimerSleep() const          { return timersleep; }
      /// Sets the time in seconds after which cleanup like checking sessiontimeout is done.
      void setTimerSleep(unsigned sec)        { timersleep = sec; }

      /// Returns the time in seconds between thread starts.
      unsigned getThreadStartDelay() const    { return threadstartdelay; }
      /// Sets the time in seconds between thread starts.
      void setThreadStartDelay(unsigned sec)  { threadstartdelay = sec; }

      /// Returns the maximum number of jobs waiting for processing.
      unsigned getQueueSize() const           { return queue.getCapacity(); }
      /// Sets the maximum number of jobs waiting for processing.
      void setQueueSize(unsigned n)           { queue.setCapacity(n); }

      /// Returns the maximum request time, after which tntnet is automatically restarted in daemon mode.
      static unsigned getMaxRequestTime();
      /// Sets the maximum request time, after which tntnet is automatically restarted in daemon mode.
      static void setMaxRequestTime(unsigned sec);

      /// Returns true, when http compression is used.
      static bool getEnableCompression();
      /// enables or disables http compression.
      static void setEnableCompression(bool sw = true);

      /// Returns the time of inactivity in seconds after which a session is destroyed
      static unsigned getSessionTimeout();
      /// Sets the time of inactivity in seconds after which a session is destroyed
      static void setSessionTimeout(unsigned sec);

      /// Returns the listen backlog parameter (see also listen(2)).
      static int getListenBacklog();
      /// Sets the listen backlog parameter (see also listen(2)).
      static void setListenBacklog(int n);

      /// Returns the number of retries, when a listen retried, when failing.
      static unsigned getListenRetry();
      /// Sets the number of retries, when a listen retried, when failing.
      static void setListenRetry(int n);

      /// Returns the maximum number of cached urlmappings.
      /// The cache stores results of the regular expressions used for defining
      /// mappings. Since the number of different urls used is normally quite
      /// limited, the cache reduces significantly the number of executed
      /// regular expressions.
      static unsigned getMaxUrlMapCache();
      /// Sets the maximum number of cached urlmappings.
      static void setMaxUrlMapCache(int n);

      /// Returns the maximum size of a request.
      /// Requestdata are collected in memory and therefore requests, which
      /// exceed the size of available memory may lead to a denial of service.
      static size_t getMaxRequestSize();
      /// Sets the maximum size of a request.
      static void setMaxRequestSize(size_t s);

      /// Returns the read timeout in millisecods after which the request is passed to the poller.
      /// Tntnet tries to keep a connection on the same thread to reduce
      /// context switches by waiting a short period if additional data
      /// arrives. After that period the request is passed to the poller,
      /// which waits for activity on the socket. The default value is
      /// 10 ms.
      static unsigned getSocketReadTimeout();
      /// Sets the timeout in millisecods after which the request is passed to the poller.
      static void setSocketReadTimeout(unsigned ms);

      /// Returns the write timeout in millisecods after which the request is timed out.
      /// The default value is 10000 ms.
      static unsigned getSocketWriteTimeout();
      /// Sets the write timeout in millisecods after which the request is timed out.
      static void setSocketWriteTimeout(unsigned ms);

      /// Returns the maximum number of requests handled over a single connection.
      /// The default value is 1000.
      static unsigned getKeepAliveMax();
      /// Sets the maximum number of requests handled over a single connection.
      static void setKeepAliveMax(unsigned ms);

      /// Returns the size of the socket buffer.
      /// This specifies the maximum number of bytes after which the data sent
      /// or received over a socket is passed to the operating system.
      /// The default value is 16384 bytes (16 kBytes).
      static unsigned getSocketBufferSize();
      /// Sets the size of the socket buffer.
      static void setSocketBufferSize(unsigned ms);

      /// Returns the minimum size of a request body for compression.
      /// Small requests are not worth compressing, so tntnet has a limit,
      /// to save cpu. The default value is 1024 bytes.
      static unsigned getMinCompressSize();
      /// Sets the minimum size of a request body for compression.
      static void setMinCompressSize(unsigned s);

      /// Returns the keep alive timeout in milliseconds.
      /// This specifies, how long a connection is kept for keep alive. A keep
      /// alive request binds (little) resources, so it is good to free it
      /// after some time of inactivity. The default value if 15000 ms.
      static unsigned getKeepAliveTimeout();
      /// Sets the keep alive timeout in milliseconds.
      static void setKeepAliveTimeout(unsigned s);

      /// Returns the default content type.
      /// The default content type is "text/html; charset=iso-8859-1".
      static const std::string& getDefaultContentType();
      /// Sets the default content type.
      static void setDefaultContentType(const std::string& s);

      /// Adds a file path where components are searched.
      static void addSearchPathEntry(const std::string& path);

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

