/* tntnet.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/tntnet.h"
#include "tnt/worker.h"
#include "tnt/listener.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/sessionscope.h"

#include <cxxtools/tcpstream.h>
#include <cxxtools/log.h>

#include <config.h>

#ifndef TNTNET_CONF
# define TNTNET_CONF "/etc/tntnet.conf"
#endif

#ifndef TNTNET_PID
# define TNTNET_PID "/var/run/tntnet.pid"
#endif

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
          throw std::runtime_error(msg.str());
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
          throw std::runtime_error(msg.str());
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
}

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // Tntnet
  //
  Tntnet::Tntnet()
    : pollerthread(queue),
      minthreads(5),
      maxthreads(100),
      threadstartdelay(10),
      timersleep(10)
  { }

  bool Tntnet::stop = false;

  void Tntnet::init(const Tntconfig& config)
  {
    minthreads = config.getValue<unsigned>("MinThreads", minthreads);
    maxthreads = config.getValue<unsigned>("MaxThreads", maxthreads);
    threadstartdelay = config.getValue<unsigned>("ThreadStartDelay", threadstartdelay);
    timersleep = config.getValue<unsigned>("TimerSleep", timersleep);
    Worker::setMaxRequestTime(config.getValue<unsigned>("MaxRequestTime", Worker::getMaxRequestTime()));
    Worker::setEnableCompression(config.getBoolValue("EnableCompression", Worker::getEnableCompression()));
    queue.setCapacity(config.getValue<unsigned>("QueueSize", queue.getCapacity()));
    Sessionscope::setDefaultTimeout(config.getValue<unsigned>("SessionTimeout", Sessionscope::getDefaultTimeout()));
    Listener::setBacklog(config.getValue<int>("ListenBacklog", Listener::getBacklog()));
    Listener::setListenRetry(config.getValue<int>("ListenRetry", Listener::getListenRetry()));
    Dispatcher::setMaxUrlMapCache(config.getValue<unsigned>("MaxUrlMapCache", Dispatcher::getMaxUrlMapCache()));

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
    HttpRequest::setMaxRequestSize(
      config.getValue("MaxRequestSize", HttpRequest::getMaxRequestSize()));
    Job::setSocketReadTimeout(
      config.getValue("SocketReadTimeout", Job::getSocketReadTimeout()));
    Job::setSocketWriteTimeout(
      config.getValue("SocketWriteTimeout", Job::getSocketWriteTimeout()));
    Job::setKeepAliveMax(
      config.getValue("KeepAliveMax", Job::getKeepAliveMax()));
    Job::setSocketBufferSize(
      config.getValue("BufferSize", Job::getSocketBufferSize()));
    HttpReply::setMinCompressSize(
      config.getValue("MinCompressSize", HttpReply::getMinCompressSize()));
    HttpReply::setKeepAliveTimeout(
      config.getValue("KeepAliveTimeout", HttpReply::getKeepAliveTimeout()));
    HttpReply::setDefaultContentType(
      config.getValue("DefaultContentType", HttpReply::getDefaultContentType()));

    // initialize listeners
    Tntconfig::config_entries_type configListen;
    config.getConfigValues("Listen", configListen);

    for (Tntconfig::config_entries_type::const_iterator it = configListen.begin();
         it != configListen.end(); ++it)
    {
      if (it->params.empty())
        throw std::runtime_error("empty Listen-entry");

      unsigned short int port = 80;
      if (it->params.size() >= 2)
      {
        std::istringstream p(it->params[1]);
        p >> port;
        if (!p)
        {
          std::ostringstream msg;
          msg << "invalid port " << it->params[1];
          throw std::runtime_error(msg.str());
        }
      }

      std::string ip(it->params[0]);

      listen(ip, port);
    }

#ifdef USE_SSL
    // initialize ssl-listener
    Tntconfig::config_entries_type configSslListen;
    config.getConfigValues("SslListen", configSslListen);
    std::string defaultCertificateFile = config.getValue("SslCertificate");
    std::string defaultCertificateKey = config.getValue("SslKey");

    for (Tntconfig::config_entries_type::const_iterator it = configSslListen.begin();
         it != configSslListen.end(); ++it)
    {
      if (it->params.empty())
        throw std::runtime_error("empty SslListen-entry");

      unsigned short int port = 443;
      if (it->params.size() >= 2)
      {
        std::istringstream p(it->params[1]);
        p >> port;
        if (!p)
        {
          std::ostringstream msg;
          msg << "invalid port " << it->params[1];
          throw std::runtime_error(msg.str());
        }
      }

      std::string certificateFile =
        it->params.size() >= 3 ? it->params[2]
                               : defaultCertificateFile;
      std::string keyFile =
        it->params.size() >= 4 ? it->params[3] :
        it->params.size() >= 3 ? it->params[2] : defaultCertificateKey;

      if (certificateFile.empty())
        throw std::runtime_error("Ssl-certificate not configured");

      std::string ip(it->params[0]);

      sslListen(certificateFile, keyFile, ip, port);
    }
#endif // USE_SSL
  }

  void Tntnet::listen(const std::string& ip, unsigned short int port)
  {
    log_debug("listen on ip " << ip << " port " << port);
    listeners.insert(new tnt::Listener(ip, port, queue));
  }

  void Tntnet::sslListen(const std::string& certificateFile, const std::string& keyFile, const std::string& ip, unsigned short int port)
  {
#ifdef USE_SSL
    log_debug("listen on ip " << ip << " port " << port << " (ssl)");
    listeners.insert(new Ssllistener(certificateFile.c_str(),
        keyFile.c_str(), ip, port, queue));
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
      listeners.insert(new tnt::Listener("0.0.0.0", port, queue));
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
    pollerthread.create();

    log_debug("start timer thread");
    cxxtools::MethodThread<Tntnet, cxxtools::AttachedThread> timerThread(*this, &Tntnet::timerTask);
    timerThread.create();

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

    log_warn("stopping Tntnet");

    // join-loop
    while (!listeners.empty())
    {
      listeners_type::value_type s = *listeners.begin();
      log_debug("remove listener from listener-list");
      listeners.erase(s);

      log_debug("request listener to stop");
      s->doStop();

      delete s;

      log_debug("listener stopped");
    }

    log_info("listeners stopped");
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

    while (!stop)
    {
      sleep(timersleep);
      getScopemanager().checkSessionTimeout();
      Worker::timer();
    }

    log_warn("stopping Tntnet");

    queue.noWaitThreads.signal();
    minthreads = maxthreads = 0;
    pollerthread.doStop();
  }

  void Tntnet::shutdown()
  {
    stop = true;
  }
}
