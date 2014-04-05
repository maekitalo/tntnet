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
#include <fstream>

namespace tnt
{
  class ListenerBase;
  struct TntConfig;

  /** Main application class for stand-alone tntnet web application

      The Tntnet class is used to compile a web application into a simple executable.
      The compiled program then works as a webserver, handling the server requests
      for the web application.
      This is the alternative to compiling it into a shared library and using the
      tntnet program to handle the server requests.

      This is the minimal set of things you have to include in your code to create a
      standalone tntnet web application:
      @li The instantiation of a Tntnet object
      @li A call to listen and/or sslListen
      @li One or more calls to mapUrl or vMapUrl
      @li A call to run()

      Example:
      @code
        #include <iostream>
        #include <tnt/tntnet.h>

        int main()
        {
          try
          {
            tnt::Tntnet app;

            app.listen(8000);

            app.mapUrl("^/$", "index");
            app.mapUrl("^/(.*)$", "$1");
            app.mapUrl("^/.*$", "404");

            app.run();
          }
          catch(const std::exception& e)
          {
            std::cerr << e.what() << std::endl;
            return 1;
          }

          return 0;
        }
      @endcode
   */
  class Tntnet
  {
    friend class Worker;

    private:
      typedef std::set<ListenerBase*> listeners_type;

      unsigned _minthreads;
      unsigned _maxthreads;

      Jobqueue _queue;

      static bool _stop;

      listeners_type _listeners;
      static listeners_type _allListeners;

      cxxtools::AttachedThread _pollerthread;
      Poller _poller;
      Dispatcher _dispatcher;

      ScopeManager _scopemanager;
      std::string _appname;

      std::ofstream _accessLog;
      cxxtools::Mutex _accessLogMutex;

      // noncopyable
      Tntnet(const Tntnet&);
      Tntnet& operator= (const Tntnet&);

      void timerTask();

      static cxxtools::Condition _timerStopCondition;
      static cxxtools::Mutex _timeStopMutex;

    public:
      /** Create a %Tntnet object with default configuration

          For information on the default configuration options, see TntConfig.
       */
      Tntnet();

      /// Load server configuration from a TntConfig object
      void init(const TntConfig& config);

      /** Set up a listener for the specified ip address and port.

          An empty string means listening on all interfaces &ndash; you can simply
          use the listen() method with one parameter to do that though.

          This method solely does the setup, the actual listening starts in run().
       */
      void listen(const std::string& ipaddr, unsigned short int port);

      /** Set up a listener for the specified port which listens on all local interfaces

          As the above method, this doesn't start the actual listening.
       */
      void listen(unsigned short int port)
        { listen(std::string(), port); }

      /** Set up a ssl listener for the specified ip address and port

          See listen() for more information.
       */
      void sslListen(const std::string& certificateFile, const std::string& keyFile,
                     const std::string& ipaddr, unsigned short int port);

      /** Set up a ssl listener for the specified port which listens on all local interfaces

          See listen() for more information.
       */
      void sslListen(const std::string& certificateFile, const std::string& keyFile,
                     unsigned short int port)
        { sslListen(certificateFile, keyFile, std::string(), port); }

      /** Start all needed threads and the application loop

          If no listeners were set up through listen or sslListen, a default
          listener is instantiated. It listens on all local interfaces on either
          port 80 (when the program is executed as root) or port 8000 (other users).
       */
      void run();

      /// Request all %Tntnet instances to shut down
      static void shutdown();

      /// Tells whether a shutdown request was initiated
      static bool shouldStop()                { return _stop; }

      /// @cond internal

      Jobqueue&   getQueue()                  { return _queue; }

      Poller&     getPoller()                 { return _poller; }

      const Dispatcher& getDispatcher() const { return _dispatcher; }

      ScopeManager& getScopemanager()         { return _scopemanager; }

      /// @endcond internal

      /// Get the minimum number of worker threads
      unsigned getMinThreads() const          { return _minthreads; }

      /// Set the minimum number of worker threads
      void setMinThreads(unsigned n);

      /// Get the maximum number of worker threads
      unsigned getMaxThreads() const          { return _maxthreads; }

      /// Set the maximum number of worker threads
      void setMaxThreads(unsigned n)          { _maxthreads = n; }

      /// @{
      /** @name URL mapping

          Add a mapping from a url to a component

          The matching url's are specified using a regular expression.
          The mapping target may contain back references to the expression
          using $1, $2 and so on.

          @param url The path requested by the client.
            The path includes everything between the domain name and the get parameters.
            It always begins with a '/'.

          @param ci The identifier for the component to which the url is mapped.
            The first method simply passes the string to the CompIdent constructor.
       */
      Mapping& mapUrl(const std::string& url, const std::string& ci)
        { return _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)); }

      Mapping& mapUrl(const std::string& url, const Compident& ci)
        { return _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)); }

      /// @deprecated
      void mapUrl(const std::string& url, const std::string& pathinfo, const std::string& ci)
        { _dispatcher.addUrlMapEntry(std::string(), url, Maptarget(ci)).setPathInfo(pathinfo); }

      /// @deprecated
      Mapping& mapUrl(const std::string& url, const Maptarget& ci)
        { return _dispatcher.addUrlMapEntry(std::string(), url, ci); }

      /// @deprecated
      Mapping& vMapUrl(const std::string& vhost, const std::string& url, const Maptarget& ci)
        { return _dispatcher.addUrlMapEntry(vhost, url, ci); }
      /// @}

      /** Set the app name

          The app name is used for the session cookie name if the
          web application is linked directly to a stand-alone executable.
          The name of the session cookie then is "tntnet." plus the library
          name of the web application. Since there is no library name if
          the application is run through the Tntnet application class, this
          applications name is used instead.

          Setting the app name explicitely reduces potential conflicts if
          multiple tntnet application servers are run on the same host on
          different ports.
       */
      void setAppName(const std::string& appname)
        { _appname = appname; }

      /// Get the app name &ndash; for details see setAppName()
      const std::string& getAppName() const
        { return _appname; }

      /** Enable access logging

          The used log format is the common logfile format (CLF),
          the same format Apache uses for its access logs.

          @param logfile_path The path to the log file.
       */
      void setAccessLog(const std::string& logfile_path)
        { _accessLog.open(logfile_path.c_str(), std::ios::out | std::ios::app); }
  };
}

#endif // TNT_TNTNET_H

