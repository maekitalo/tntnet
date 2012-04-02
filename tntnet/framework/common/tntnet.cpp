/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/tntnet.h"
#include "tnt/worker.h"
#include "tnt/listener.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/sessionscope.h"
#include "tnt/tntconfig.h"
#include "tnt/configurator.h"
#include "tnt/util.h"
#ifdef WITH_STRESSJOB
#include "tnt/stressjob.h"
#endif

#include <cxxtools/net/tcpstream.h>
#include <cxxtools/log.h>

#include <unistd.h>
#include <signal.h>

#include <config.h>

log_define("tntnet.tntnet")

namespace
{
  void configureDispatcher(tnt::Dispatcher& dis, const tnt::Tntconfig& config)
  {
    typedef tnt::Dispatcher::CompidentType CompidentType;

    const tnt::Tntconfig::config_entries_type& params = config.getConfigValues();

    tnt::Tntconfig::config_entries_type::const_iterator vi;
    for (vi = params.begin(); vi != params.end(); ++vi)
    {
      const tnt::Tntconfig::config_entry_type& v = *vi;
      const tnt::Tntconfig::params_type& args = v.params;
      if (v.key == "MapUrl")
      {
        if (args.size() < 2)
        {
          std::ostringstream msg;
          msg << "invalid number of parameters (" << args.size() << ") in MapUrl";
          tnt::throwRuntimeError(msg.str());
        }

        std::string url = args[0];

        CompidentType ci = CompidentType(args[1]);
        if (args.size() > 2)
        {
          ci.setPathInfo(args[2]);
          if (args.size() > 3)
            ci.setArgs(CompidentType::args_type(args.begin() + 3, args.end()));
        }

        dis.addUrlMapEntry(std::string(), url, ci);
      }
      else if (v.key == "VMapUrl")
      {
        if (args.size() < 3)
        {
          std::ostringstream msg;
          msg << "invalid number of parameters (" << args.size() << ") in VMapUrl";
          tnt::throwRuntimeError(msg.str());
        }

        std::string vhost = args[0];
        std::string url = args[1];

        CompidentType ci = CompidentType(args[2]);
        if (args.size() > 3)
        {
          ci.setPathInfo(args[3]);
          if (args.size() > 4)
            ci.setArgs(CompidentType::args_type(args.begin() + 4, args.end()));
        }

        dis.addUrlMapEntry(vhost, url, ci);
      }
    }
  }

  typedef std::set<tnt::Tntnet*> TntnetInstancesType;
  cxxtools::Mutex allTntnetInstancesMutex;
  TntnetInstancesType allRunningTntnetInstances;

}

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // Tntnet
  //
  Tntnet::Tntnet()
    : minthreads(5),
      maxthreads(100),
      threadstartdelay(10),
      timersleep(10),
      pollerthread(cxxtools::callable(poller, &Poller::run)),
      poller(queue)
  { }

  bool Tntnet::stop = false;
  cxxtools::Mutex Tntnet::timeStopMutex;
  cxxtools::Condition Tntnet::timerStopCondition;

  
  Tntnet::listeners_type Tntnet::allListeners;

  void Tntnet::init(const Tntconfig& config)
  {
    Configurator c(*this);
    c.setMinThreads(        config.getValue("MinThreads",            c.getMinThreads()));
    c.setMaxThreads(        config.getValue("MaxThreads",            c.getMaxThreads()));
    c.setThreadStartDelay(  config.getValue("ThreadStartDelay",      c.getThreadStartDelay()));
    c.setTimerSleep(        config.getValue("TimerSleep",            c.getTimerSleep()));
    c.setMaxRequestTime(    config.getValue("MaxRequestTime",        c.getMaxRequestTime()));
    c.setEnableCompression( config.getBoolValue("EnableCompression", c.getEnableCompression()));
    c.setQueueSize(         config.getValue("QueueSize",             c.getQueueSize()));
    c.setSessionTimeout(    config.getValue("SessionTimeout",        c.getSessionTimeout()));
    c.setListenBacklog(     config.getValue("ListenBacklog",         c.getListenBacklog()));
    c.setListenRetry(       config.getValue("ListenRetry",           c.getListenRetry()));
    c.setMaxUrlMapCache(    config.getValue("MaxUrlMapCache",        c.getMaxUrlMapCache()));
    std::string accessLog_ = config.getValue("AccessLog");
    if (!accessLog_.empty())
      c.setAccessLog(accessLog_);
    c.setMaxBackgroundTasks(config.getValue("MaxBackgroundTasks",    c.getMaxBackgroundTasks()));

    Tntconfig::config_entries_type configSetEnv;
    config.getConfigValues("SetEnv", configSetEnv);
    for (Tntconfig::config_entries_type::const_iterator it = configSetEnv.begin();
         it != configSetEnv.end(); ++it)
    {
      if (it->params.size() >= 2)
      {
#ifdef HAVE_SETENV
        log_debug("setenv " << it->params[0] << "=\"" << it->params[1] << '"');
        ::setenv(it->params[0].c_str(), it->params[1].c_str(), 1);
#else
        std::string name  = it->params[0];
        std::string value = it->params[1];

        char* env = new char[name.size() + value.size() + 2];
        name.copy(env, name.size());
        env[name.size()] = '=';
        value.copy(env + name.size() + 1, value.size());
        env[name.size() + value.size() + 1] = '\0';

        log_debug("putenv(" << env << ')');
        ::putenv(env);
#endif
      }
    }

    configureDispatcher(dispatcher, config);

    // configure worker (static)
    Comploader::configure(config);

    // configure http
    c.setMaxRequestSize(    config.getValue("MaxRequestSize",      c.getMaxRequestSize()));
    c.setSocketReadTimeout( config.getValue("SocketReadTimeout",   c.getSocketReadTimeout()));
    c.setSocketWriteTimeout(config.getValue("SocketWriteTimeout",  c.getSocketWriteTimeout()));
    c.setKeepAliveMax(      config.getValue("KeepAliveMax",        c.getKeepAliveMax()));
    c.setSocketBufferSize(  config.getValue("BufferSize",          c.getSocketBufferSize()));
    c.setMinCompressSize(   config.getValue("MinCompressSize",     c.getMinCompressSize()));
    c.setKeepAliveTimeout(  config.getValue("KeepAliveTimeout",    c.getKeepAliveTimeout()));
    c.setDefaultContentType(config.getValue("DefaultContentType",  c.getDefaultContentType()));

    // initialize listeners
    Tntconfig::config_entries_type configListen;
    config.getConfigValues("Listen", configListen);

    for (Tntconfig::config_entries_type::const_iterator it = configListen.begin();
         it != configListen.end(); ++it)
    {
      if (it->params.empty())
        throwRuntimeError("empty Listen-entry");

      unsigned short int port = 80;
      if (it->params.size() >= 2)
      {
        std::istringstream p(it->params[1]);
        p >> port;
        if (!p)
        {
          std::ostringstream msg;
          msg << "invalid port " << it->params[1];
          throwRuntimeError(msg.str());
        }
      }

      std::string ip(it->params[0]);

      listen(ip, port);
    }

    Tntconfig::config_entries_type configSslListen;
    config.getConfigValues("SslListen", configSslListen);

#ifdef USE_SSL

    // initialize ssl-listener
    std::string defaultCertificateFile = config.getValue("SslCertificate");
    std::string defaultCertificateKey = config.getValue("SslKey");

    for (Tntconfig::config_entries_type::const_iterator it = configSslListen.begin();
         it != configSslListen.end(); ++it)
    {
      if (it->params.empty())
        throwRuntimeError("empty SslListen-entry");

      unsigned short int port = 443;
      if (it->params.size() >= 2)
      {
        std::istringstream p(it->params[1]);
        p >> port;
        if (!p)
        {
          std::ostringstream msg;
          msg << "invalid port " << it->params[1];
          throwRuntimeError(msg.str());
        }
      }

      std::string certificateFile =
        it->params.size() >= 3 ? it->params[2]
                               : defaultCertificateFile;
      std::string keyFile =
        it->params.size() >= 4 ? it->params[3] :
        it->params.size() >= 3 ? it->params[2] : defaultCertificateKey;

      if (certificateFile.empty())
        throwRuntimeError("Ssl-certificate not configured");

      std::string ip(it->params[0]);

      sslListen(certificateFile, keyFile, ip, port);
    }

#else

    if (!configSslListen.empty())
      throwRuntimeError("SslListen is configured but SSL is not compiled into this tntnet");

#endif // USE_SSL

#ifdef WITH_STRESSJOB
    std::string stressJob = config.getValue("StressJob");
    if (!stressJob.empty())
    {
      log_debug("create stress job for url \"" << stressJob << '"');
      queue.put(new StressJob(*this, stressJob));
    }
#endif
  }

  void Tntnet::listen(const std::string& ip, unsigned short int port)
  {
    log_debug("listen on ip " << ip << " port " << port);
    ListenerBase* listener = new tnt::Listener(*this, ip, port, queue);
    listeners.insert(listener);
    allListeners.insert(listener);
  }

  void Tntnet::sslListen(const std::string& certificateFile, const std::string& keyFile, const std::string& ip, unsigned short int port)
  {
#ifdef USE_SSL
    log_debug("listen on ip " << ip << " port " << port << " (ssl)");
    ListenerBase* listener = new Ssllistener(*this, certificateFile.c_str(),
        keyFile.c_str(), ip, port, queue);
    listeners.insert(listener);
    allListeners.insert(listener);
#else
    log_error("cannot add ssl listener - ssl is not compiled into tntnet");
#endif // USE_SSL
  }

  void Tntnet::run()
  {
    log_debug("worker-process");

    stop = false;

    if (listeners.empty())
    {
      unsigned short int port = (getuid() == 0 ? 80 : 8000);
      log_info("no listeners defined - using ip 0.0.0.0 port " << port);
      listen("0.0.0.0", port);
    }
    else
      log_debug(listeners.size() << " listeners");

    if (listeners.size() >= minthreads)
    {
      log_warn("at least one more worker than listeners needed - set MinThreads to "
        << listeners.size() + 1);
      minthreads = listeners.size() + 1;
    }

    if (maxthreads < minthreads)
    {
      log_warn("MaxThreads < MinThreads - set MaxThreads = MinThreads = " << minthreads);
      maxthreads = minthreads;
    }

    // initialize worker-process

    // set FD_CLOEXEC
    for (listeners_type::iterator it = listeners.begin(); it != listeners.end(); ++it)
      (*it)->initialize();

    // SIGPIPE must be ignored
    ::signal(SIGPIPE, SIG_IGN);

    // create worker-threads
    log_info("create " << minthreads << " worker threads");
    for (unsigned i = 0; i < minthreads; ++i)
    {
      log_debug("create worker " << i);
      Worker* s = new Worker(*this);
      s->create();
    }

    // create poller-thread
    log_debug("start poller thread");
    pollerthread.start();

    log_debug("start timer thread");
    cxxtools::AttachedThread timerThread(cxxtools::callable(*this, &Tntnet::timerTask));
    timerThread.start();

    {
      cxxtools::MutexLock lock(allTntnetInstancesMutex);
      allRunningTntnetInstances.insert(this);
    }

    // mainloop
    cxxtools::Mutex mutex;
    while (!stop)
    {
      {
        cxxtools::MutexLock lock(mutex);
        queue.noWaitThreads.wait(lock);
      }

      if (stop)
        break;

      if (Worker::getCountThreads() < maxthreads)
      {
        log_info("create workerthread");
        Worker* s = new Worker(*this);
        s->create();
      }
      else
        log_info("max worker-threadcount " << maxthreads << " reached");

      if (threadstartdelay > 0)
        usleep(threadstartdelay * 1000);
    }

    log_info("stopping Tntnet");

    {
      cxxtools::MutexLock lock(allTntnetInstancesMutex);
      allRunningTntnetInstances.erase(this);
    }

    log_info("wake listeners");
    for (listeners_type::iterator it = listeners.begin(); it != listeners.end(); ++it)
      (*it)->doStop();

    log_info("stop poller thread");
    poller.doStop();
    pollerthread.join();

    log_info("stop timer thread");
    timerThread.join();

    if (Worker::getCountThreads() > 0)
    {
      log_info("wait for " << Worker::getCountThreads() << " worker threads to stop");
      while (Worker::getCountThreads() > 0)
      {
        log_debug("wait for worker threads to stop; " << Worker::getCountThreads() << " left");
        usleep(100);
      }
    }

    log_debug("destroy listeners");
    for (listeners_type::iterator it = listeners.begin(); it != listeners.end(); ++it)
      delete *it;
    listeners.clear();

    log_info("all threads stopped");
  }

  void Tntnet::setMinThreads(unsigned n)
  {
    if (listeners.size() >= n)
    {
      log_warn("at least one more worker than listeners needed - set MinThreads to "
        << listeners.size() + 1);
      minthreads = listeners.size() + 1;
    }
    else
      minthreads = n;

  }

  void Tntnet::timerTask()
  {
    log_debug("timer thread");

    while (true)
    {
      {
        cxxtools::MutexLock timeStopLock(timeStopMutex);
        if (stop || timerStopCondition.wait(timeStopLock, timersleep * 1000))
          break;
      }

      getScopemanager().checkSessionTimeout();
      Worker::timer();
    }

    queue.noWaitThreads.signal();
    minthreads = maxthreads = 0;
  }

  void Tntnet::shutdown()
  {
    stop = true;
    timerStopCondition.broadcast();
  }

}
