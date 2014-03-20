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

namespace tnt
{
  namespace
  {
    void configureDispatcher(Dispatcher& dis)
    {
      const TntConfig::MappingsType& mappings = TntConfig::it().mappings;
      for (TntConfig::MappingsType::const_iterator it = mappings.begin(); it != mappings.end(); ++it)
      {
        Maptarget ci(it->target);
        if (!it->pathinfo.empty())
          ci.setPathInfo(it->pathinfo);
        ci.setArgs(it->args);
        dis.addUrlMapEntry(it->vhost, it->url, it->method, it->ssl, ci);
      }
    }

    typedef std::set<Tntnet*> TntnetInstancesType;
    cxxtools::Mutex allTntnetInstancesMutex;
    TntnetInstancesType allRunningTntnetInstances;

  }

  ////////////////////////////////////////////////////////////////////////
  // Tntnet
  //
  Tntnet::Tntnet()
    : _minthreads(TntConfig::it().minThreads),
      _maxthreads(TntConfig::it().maxThreads),
      _pollerthread(cxxtools::callable(_poller, &Poller::run)),
      _poller(_queue)
  { }

  bool Tntnet::_stop = false;
  cxxtools::Mutex Tntnet::_timeStopMutex;
  cxxtools::Condition Tntnet::_timerStopCondition;


  Tntnet::listeners_type Tntnet::_allListeners;

  void Tntnet::init(const TntConfig& config)
  {
    _minthreads = config.minThreads;
    _maxthreads = config.maxThreads;

    _queue.setCapacity(config.queueSize);

    for (TntConfig::EnvironmentType::const_iterator it = config.environment.begin(); it != config.environment.end(); ++it)
    {
      std::string name = it->first;
      std::string value = it->second;
#ifdef HAVE_SETENV
      log_debug("setenv " << name << "=\"" << value << '"');
      ::setenv(name.c_str(), value.c_str(), 1);
#else
      char* env = new char[name.size() + value.size() + 2];
      name.copy(env, name.size());
      env[name.size()] = '=';
      value.copy(env + name.size() + 1, value.size());
      env[name.size() + value.size() + 1] = '\0';

      log_debug("putenv(" << env << ')');
      ::putenv(env);
#endif
    }

    configureDispatcher(_dispatcher);

    // initialize listeners
    for (TntConfig::ListenersType::const_iterator it = config.listeners.begin(); it != config.listeners.end(); ++it)
    {
      listen(it->ip, it->port);
    }

#ifdef USE_SSL

    for (TntConfig::SslListenersType::const_iterator it = config.ssllisteners.begin(); it != config.ssllisteners.end(); ++it)
    {
      sslListen(it->certificate, it->key, it->ip, it->port);
    }

#else

    if (!config.ssllisteners.empty())
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
    ListenerBase* listener = new Listener(*this, ip, port, _queue);
    _listeners.insert(listener);
    _allListeners.insert(listener);
  }

  void Tntnet::sslListen(const std::string& certificateFile, const std::string& keyFile, const std::string& ip, unsigned short int port)
  {
#ifdef USE_SSL
    log_debug("listen on ip " << ip << " port " << port << " (ssl)");
    ListenerBase* listener = new Ssllistener(*this, certificateFile.c_str(),
        keyFile.c_str(), ip, port, _queue);
    _listeners.insert(listener);
    _allListeners.insert(listener);
#else
    log_error("cannot add ssl listener - ssl is not compiled into tntnet");
#endif // USE_SSL
  }

  void Tntnet::run()
  {
    log_debug("worker-process");

    _stop = false;

    if (_listeners.empty())
      throwRuntimeError("no listeners defined");

    log_debug(_listeners.size() << " listeners");

    if (_listeners.size() >= _minthreads)
    {
      log_warn("at least one more worker than listeners needed - set MinThreads to "
        << _listeners.size() + 1);
      _minthreads = _listeners.size() + 1;
    }

    if (_maxthreads < _minthreads)
    {
      log_warn("MaxThreads < MinThreads - set MaxThreads = MinThreads = " << _minthreads);
      _maxthreads = _minthreads;
    }

    // initialize worker-process

    // set FD_CLOEXEC
    for (listeners_type::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
      (*it)->initialize();

    // SIGPIPE must be ignored
    ::signal(SIGPIPE, SIG_IGN);

    // create worker-threads
    log_info("create " << _minthreads << " worker threads");
    for (unsigned i = 0; i < _minthreads; ++i)
    {
      log_debug("create worker " << i);
      Worker* s = new Worker(*this);
      s->create();
    }

    // create poller-thread
    log_debug("start poller thread");
    _pollerthread.start();

    log_debug("start timer thread");
    cxxtools::AttachedThread timerThread(cxxtools::callable(*this, &Tntnet::timerTask));
    timerThread.start();

    {
      cxxtools::MutexLock lock(allTntnetInstancesMutex);
      allRunningTntnetInstances.insert(this);
    }

    // mainloop
    cxxtools::Mutex mutex;
    while (!_stop)
    {
      {
        cxxtools::MutexLock lock(mutex);
        _queue.noWaitThreads.wait(lock);
      }

      if (_stop)
        break;

      if (Worker::getCountThreads() < _maxthreads)
      {
        log_info("create workerthread");
        Worker* s = new Worker(*this);
        s->create();
      }
      else
        log_info("max worker-threadcount " << _maxthreads << " reached");

      if (TntConfig::it().threadStartDelay > 0)
        usleep(TntConfig::it().threadStartDelay * 1000);
    }

    log_info("stopping Tntnet");

    {
      cxxtools::MutexLock lock(allTntnetInstancesMutex);
      allRunningTntnetInstances.erase(this);
    }

    log_info("stop listener");
    for (listeners_type::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
      (*it)->doTerminate();

    log_info("stop poller thread");
    _poller.doStop();
    _pollerthread.join();

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

    log_debug("destroy listener");
    for (listeners_type::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
      delete *it;
    _listeners.clear();

    HttpReply::postRunCleanup();
    HttpRequest::postRunCleanup();

    log_info("all threads stopped");
  }

  void Tntnet::setMinThreads(unsigned n)
  {
    if (_listeners.size() >= n)
    {
      log_warn("at least one more worker than listeners needed - set MinThreads to "
        << _listeners.size() + 1);
      _minthreads = _listeners.size() + 1;
    }
    else
      _minthreads = n;

  }

  void Tntnet::timerTask()
  {
    log_debug("timer thread");

    while (true)
    {
      {
        cxxtools::MutexLock timeStopLock(_timeStopMutex);
        if (_stop || _timerStopCondition.wait(timeStopLock, TntConfig::it().timerSleep * 1000))
          break;
      }

      getScopemanager().checkSessionTimeout();
      Worker::timer();
    }

    _queue.noWaitThreads.signal();
    _minthreads = _maxthreads = 0;
  }

  void Tntnet::shutdown()
  {
    _stop = true;
    _timerStopCondition.broadcast();
  }

}

