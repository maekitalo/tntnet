/* tntnet.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#include "tnt/worker.h"
#include "tnt/tntnet.h"
#include "tnt/listener.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/sessionscope.h"

#include <cxxtools/tcpstream.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <config.h>

#ifndef TNTNET_CONF
# define TNTNET_CONF "/etc/tntnet.conf"
#endif

#ifndef TNTNET_PID
# define TNTNET_PID "/var/run/tntnet.pid"
#endif

log_define("tntnet.tntnet")

#define log_error_master(expr) \
  do { \
    std::cout << "ERROR: " << expr << std::endl; \
  } while(false)

#define log_warn_master(expr) \
  do { \
    std::cout << "WARN: " << expr << std::endl; \
  } while(false)

#define log_info_master(expr) \
  do { \
    std::cout << "INFO: " << expr << std::endl; \
  } while(false)

#define log_debug_master(expr) \
  do { \
    std::cout << "DEBUG: " << expr << std::endl; \
  } while(false)

namespace
{
  void sigEnd(int)
  {
    tnt::Tntnet::shutdown();
  }

  void sigReload(int)
  {
    // stopping child with 111 signals monitor-process to restart child
    tnt::Tntnet::restart();
  }

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

  bool checkChildSuccess(int fd)
  {
    log_debug("checkChildSuccess");

    char buffer;
    int ret = ::read(fd, &buffer, 1);
    if (ret < 0)
      throw std::runtime_error(
        std::string("error in read: ") + strerror(errno));
    close(fd);
    return ret > 0;
  }

  void signalParentSuccess(int fd)
  {
    log_debug("signalParentSuccess");

    ssize_t s = write(fd, "1", 1);
    if (s < 0)
      throw std::runtime_error(
        std::string("error in write(): ") + strerror(errno));
    close(fd);
  }

}

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // Tntnet
  //
  bool Tntnet::stop = false;
  int Tntnet::ret = 0;
  std::string Tntnet::pidFileName;

  Tntnet::Tntnet(int& argc, char* argv[])
    : propertyfilename(argc, argv, 'P'),
      debug(argc, argv, 'd'),
      queue(1000),
      pollerthread(queue)
  {
    // check for argument -c
    cxxtools::Arg<const char*> conf(argc, argv, 'c');
    if (conf.isSet())
      configFile = conf;
    else
    {
      // read 1st parameter from argument-list
      cxxtools::Arg<const char*> conf(argc, argv);
      if (conf.isSet())
        configFile = conf;
      else
      {
        // check environment-variable TNTNET_CONF
        const char* tntnetConf = ::getenv("TNTNET_CONF");
        if (tntnetConf)
          configFile = tntnetConf;
        else if (getuid() != 0)
          configFile = "tntnet.conf";
        else
          configFile = TNTNET_CONF;  // take default
      }
    }
  }

  void Tntnet::setGroup() const
  {
    Tntconfig::params_type group = config.getConfigValue("Group");
    if (group.size() >= 1)
    {
      struct group * gr = getgrnam(group.begin()->c_str());
      if (gr == 0)
        throw std::runtime_error("unknown group " + *group.begin());

      log_debug("change group to " << *group.begin() << '(' << gr->gr_gid << ')');

      int ret = setgid(gr->gr_gid);
      if (ret != 0)
      {
        std::ostringstream msg;
        msg << "cannot change group to " << *group.begin()
            << '(' << gr->gr_gid << "): " << strerror(errno);
        throw std::runtime_error(msg.str());
      }
    }
  }

  void Tntnet::setDir(const char* def) const
  {
    std::string dir = config.getValue("Dir", def);

    if (!dir.empty())
    {
      log_debug("chdir(" << dir << ')');
      if (chdir(dir.c_str()) == -1)
      {
        throw std::runtime_error(
          std::string("error in chdir(): ")
            + strerror(errno));
      }
    }

    std::string chrootdir = config.getValue("Chroot");
    if (!chrootdir.empty() && chroot(chrootdir.c_str()) == -1)
      throw std::runtime_error(
        std::string("error in chroot(): ")
          + strerror(errno));
  }

  void Tntnet::setUser() const
  {
    Tntconfig::params_type user = config.getConfigValue("User");
    if (user.size() >= 1)
    {
      struct passwd * pw = getpwnam(user.begin()->c_str());
      if (pw == 0)
        throw std::runtime_error("unknown user " + *user.begin());

      log_debug("change user to " << *user.begin() << '(' << pw->pw_uid << ')');

      int ret = setuid(pw->pw_uid);
      if (ret != 0)
      {
        std::ostringstream msg;
        msg << "cannot change user to " << *user.begin()
            << '(' << pw->pw_uid << "): " << strerror(errno);
        throw std::runtime_error(msg.str());
      }
    }
  }

  int Tntnet::mkDaemon() const
  {
    log_info("start daemon-mode");

    int filedes[2];

    if (pipe(filedes) != 0)
      throw std::runtime_error(
        std::string("error in pipe(int[2]): ") + strerror(errno));

    int pid = fork();
    if (pid > 0)
    {
      // parent

      close(filedes[1]); // close write-fd

      // exit with error, when nothing read
      ::exit (checkChildSuccess(filedes[0]) ? 0 : 1);
    }
    else if (pid < 0)
      throw std::runtime_error(
        std::string("error in fork(): ") + strerror(errno));

    // child

    close(filedes[0]); // close read-fd

    // setsid
    if (setsid() == -1)
      throw std::runtime_error(
        std::string("error in setsid(): ")
          + strerror(errno));

    // return write-fd
    return filedes[1];
  }

  void Tntnet::closeStdHandles() const
  {
    // close stdin, stdout and stderr
    bool noclosestd = config.getBoolValue("NoCloseStdout", false);
    if (noclosestd)
    {
      log_debug("not closing stdout");
      return;
    }

    if (freopen("/dev/null", "r", stdin) == 0)
      throw std::runtime_error(
        std::string("unable to replace stdin with /dev/null: ")
          + strerror(errno));

    if (freopen("/dev/null", "w", stdout) == 0)
      throw std::runtime_error(
        std::string("unable to replace stdout with /dev/null: ")
          + strerror(errno));

    if (freopen("/dev/null", "w", stderr) == 0)
      throw std::runtime_error(
        std::string("unable to replace stderr with /dev/null: ")
          + strerror(errno));
  }

  int Tntnet::run()
  {
    loadConfiguration();

    if (debug)
    {
      log_init_debug();
      log_warn("Debugmode");
      isDaemon = false;
    }
    else
    {
      isDaemon = config.getBoolValue("Daemon", false);
    }

    if (isDaemon)
    {
      int filedes = mkDaemon();

      setDir("");

      bool nomonitor = config.getBoolValue("NoMonitor", false);
      if (nomonitor)
      {
        log_debug("start worker-process without monitor");
        writePidfile(getpid());
        initWorkerProcess();

        // change group and user
        setGroup();
        setUser();

        initLogging();
        workerProcess(filedes);
      }
      else
      {
        initWorkerProcess();
        do
        {
          int filedes_monitor[2];

          if (pipe(filedes_monitor) != 0)
            throw std::runtime_error(
              std::string("error in pipe(int[2]): ") + strerror(errno));

          // fork workerprocess
          int pid = fork();
          if (pid < 0)
            throw std::runtime_error(
              std::string("error in forking workerprocess: ")
                + strerror(errno));

          if (pid == 0)
          {
            // workerprocess

            close(filedes_monitor[0]);  // close read-fd

            // change group and user
            setGroup();
            setUser();

            initLogging();
            workerProcess(filedes_monitor[1]);
            return ret;
          }
          else
          {
            close(filedes_monitor[1]);  // close write-fd

            // write child-pid
            writePidfile(pid);

            // wait for worker to signal success
            if (!checkChildSuccess(filedes_monitor[0]))
              ::exit(1);
            if (filedes >= 0)
            {
              signalParentSuccess(filedes);
              filedes = -1;
            }

            monitorProcess(pid);
            if (!stop)
              sleep(1);
          }

        } while (!stop);
      }
    }
    else
    {
      log_info("no daemon-mode");
      initLogging();
      initWorkerProcess();
      workerProcess();
    }

    return 0;
  }

  void Tntnet::initLogging()
  {
    if (debug)
      return;  // logging already initialized

    std::string pf;
    if (propertyfilename.isSet())
      pf = propertyfilename.getValue();
    else
      pf = config.getValue("PropertyFile");

    if (pf.empty())
      log_init();
    else
    {
      struct stat properties_stat;
      if (stat(pf.c_str(), &properties_stat) != 0)
        throw std::runtime_error("propertyfile " + pf + " not found");

      log_init(pf.c_str());
    }
  }

  void Tntnet::writePidfile(int pid)
  {
    pidFileName = config.getValue("PidFile", TNTNET_PID);

    log_debug("pidfile=" << pidFileName);

    if (!pidFileName.empty())
    {
      if (pidFileName[0] != '/')
      {
        // prepend current working-directory to pidfilename if not absolute
        std::vector<char> buf(256);
        const char* cwd;
        while (true)
        {
          cwd = ::getcwd(&buf[0], buf.size());
          if (cwd)
            break;
          else if (errno == ERANGE)
            buf.resize(buf.size() * 2);
          else
            throw std::runtime_error(
              std::string("error in getcwd: ") + strerror(errno));
        }
        pidFileName = std::string(cwd) + '/' + pidFileName;
        log_debug("pidfile=" << pidFileName);
      }

      std::ofstream pidfile(pidFileName.c_str());
      if (!pidfile)
        throw std::runtime_error("unable to open pid-file " + pidFileName);
      pidfile << pid;
    }
  }

  void Tntnet::monitorProcess(int workerPid)
  {
    setDir("");

    // close stdin, stdout and stderr
    closeStdHandles();

    int status;
    waitpid(workerPid, &status, 0);

    if (WIFSIGNALED(status))
    {
      // SIGTERM means normal exit
      if (WTERMSIG(status) == SIGTERM)
      {
        log_info_master("child terminated normally");
        stop = true;
      }
      else
      {
        log_warn_master("child terminated with signal "
          << WTERMSIG(status) << " - restart child");
      }
    }
    else if (WEXITSTATUS(status) == 111)
    {
      log_info_master("child requested restart");
    }
    else
    {
      log_info_master("child exited with exitcode " << WEXITSTATUS(status));
      stop = true;
    }

    if (unlink(pidFileName.c_str()) != 0)
      log_error_master("failed to remove pidfile \"" << pidFileName << "\" error " << errno);
  }

  void Tntnet::initWorkerProcess()
  {
    log_debug("init workerprocess");

    signal(SIGPIPE, SIG_IGN);
    signal(SIGABRT, SIG_IGN);
    signal(SIGTERM, sigEnd);
    signal(SIGHUP, sigReload);

    configureDispatcher(d_dispatcher, config);

    // create listener-threads
    Tntconfig::config_entries_type configListen;
    config.getConfigValues("Listen", configListen);

    if (configListen.empty())
    {
      unsigned short int port = (getuid() == 0 ? 80 : 8000);
      log_info("no listeners defined - using 0.0.0.0:" << port);
      ListenerBase* s = new tnt::Listener("0.0.0.0", port, queue);
      listeners.insert(s);
    }
    else
    {
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
        log_debug("create listener ip=" << ip << " port=" << port);
        ListenerBase* s = new tnt::Listener(ip, port, queue);
        listeners.insert(s);
      }
    }

#ifdef USE_SSL
    // create ssl-listener-threads
    std::string defaultCertificateFile = config.getValue("SslCertificate");
    std::string defaultCertificateKey = config.getValue("SslKey");
    configListen.clear();
    config.getConfigValues("SslListen", configListen);

    for (Tntconfig::config_entries_type::const_iterator it = configListen.begin();
         it != configListen.end(); ++it)
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
      std::string certificateKey =
        it->params.size() >= 4 ? it->params[3] :
        it->params.size() >= 3 ? it->params[2] : defaultCertificateKey;

      if (certificateFile.empty())
        throw std::runtime_error("Ssl-certificate not configured");

      std::string ip(it->params[0]);
      log_debug("create ssl-listener ip=" << ip << " port=" << port);
      ListenerBase* s = new Ssllistener(certificateFile.c_str(),
          certificateKey.c_str(), ip, port, queue);
      listeners.insert(s);
    }
#endif // USE_SSL

    // configure worker (static)
    Comploader::configure(config);

    // configure http
    HttpMessage::setMaxRequestSize(
      config.getValue("MaxRequestSize", HttpMessage::getMaxRequestSize()));
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

    log_debug("listeners.size()=" << listeners.size());
  }

  void Tntnet::workerProcess(int filedes)
  {
    log_debug("worker-process");

    // reload configuration
    config = Tntconfig();
    loadConfiguration();

    // initialize worker-process
    minthreads = config.getValue<unsigned>("MinThreads", 5);
    maxthreads = config.getValue<unsigned>("MaxThreads", 100);
    threadstartdelay = config.getValue<unsigned>("ThreadStartDelay", 10);
    Worker::setMinThreads(minthreads);
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

        log_debug("putenv(" << env);
        ::putenv(env);
#endif
      }
    }

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

    // launch listener-threads
    log_info("create " << listeners.size() << " listener threads");
    for (listeners_type::iterator it = listeners.begin();
         it != listeners.end(); ++it)
      (*it)->create();

    log_debug("start timer thread");
    cxxtools::MethodThread<Tntnet, cxxtools::AttachedThread> timerThread(*this, &Tntnet::timerTask);
    timerThread.create();

    if (filedes >= 0)
    {
      signalParentSuccess(filedes);
      closeStdHandles();
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
        log_warn("max worker-threadcount " << maxthreads << " reached");

      if (threadstartdelay > 0)
        usleep(threadstartdelay);
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

      log_debug("join listener-thread");
      s->join();
      delete s;

      log_debug("listener stopped");
    }

    log_info("listeners stopped");
  }

  void Tntnet::timerTask()
  {
    log_debug("timer thread");

    while (!stop)
    {
      sleep(1);

      log_debug("check sessiontimeout");
      getScopemanager().checkSessionTimeout();

      log_debug("worker-timer");
      Worker::timer();
    }

    log_warn("stopping Tntnet");

    if (!pidFileName.empty())
      unlink(pidFileName.c_str());

    queue.noWaitThreads.signal();
    Worker::setMinThreads(0);
    pollerthread.doStop();
  }

  void Tntnet::loadConfiguration()
  {
    config = Tntconfig();
    config.load(configFile.c_str());
  }

  void Tntnet::shutdown()
  {
    stop = true;
  }

  void Tntnet::restart()
  {
    // stopping child with 111 signals monitor-process to restart child
    stop = true;
    ret = 111;
  }

}

////////////////////////////////////////////////////////////////////////
// main
//

int main(int argc, char* argv[])
{
  signal(SIGPIPE, SIG_IGN);
  signal(SIGABRT, SIG_IGN);
  signal(SIGTERM, sigEnd);
  std::ios::sync_with_stdio(false);

  try
  {
    tnt::Tntnet app(argc, argv);
    if (argc != 1)
    {
      std::cout << PACKAGE_STRING "\n\n" <<
             "usage: " << argv[0] << " {options}\n\n"
             "  -c file          configurationfile (default: " TNTNET_CONF ")\n"
             "  -d               enable all debug output (ignoring properties-file)\n";
      return -1;
    }

    return app.run();
  }
  catch(const std::exception& e)
  {
    log_fatal(e.what());
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
