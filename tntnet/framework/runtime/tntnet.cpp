/* tntnet.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include "tnt/worker.h"
#include "tnt/tntnet.h"
#include "tnt/dispatcher.h"
#include "tnt/listener.h"
#include "tnt/http.h"

#include <cxxtools/tcpstream.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <config.h>

#ifndef CONFIG_DIR
# define CONFIG_DIR "/etc/tntnet/"
#endif

#ifndef TNTNET_CONF
# define TNTNET_CONF CONFIG_DIR "tntnet.conf"
#endif

#ifndef TNTNET_PID
# define TNTNET_PID "/var/run/tntnet.pid"
#endif

log_define("tntnet.tntnet");

namespace
{
  void configureDispatcher(tnt::dispatcher& dis, const tnt::tntconfig& config)
  {
    typedef tnt::dispatcher::compident_type compident_type;

    const tnt::tntconfig::config_entries_type& params = config.getConfigValues();

    tnt::tntconfig::config_entries_type::const_iterator vi;
    for (vi = params.begin(); vi != params.end(); ++vi)
    {
      const tnt::tntconfig::config_entry_type& v = *vi;
      const tnt::tntconfig::params_type& args = v.params;
      if (v.key == "MapUrl")
      {
        if (args.size() < 2)
        {
          std::ostringstream msg;
          msg << "invalid number of parameters (" << args.size() << ") in MapUrl";
          throw std::runtime_error(msg.str());
        }

        std::string url = args[0];

        compident_type ci = compident_type(args[1]);
        if (args.size() > 2)
        {
          ci.setPathInfo(args[2]);
          if (args.size() > 3)
            ci.setArgs(compident_type::args_type(args.begin() + 3, args.end()));
        }

        dis.addUrlMapEntry(url, ci);
      }
    }
  }
}

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // tntnet
  //
  bool tntnet::stop = false;
  std::string tntnet::pidFileName;

  tntnet::tntnet(int argc, char* argv[])
    : conf(argc, argv, 'c', TNTNET_CONF),
      propertyfilename(argc, argv, 'P'),
      debug(argc, argv, 'd'),
      pollerthread(queue),
      queue(100)
  {
    if (argc != 1)
    {
      std::ostringstream msg;
      msg << PACKAGE_STRING "\n\n"
             "usage: " << argv[0] << " {options}\n\n"
             "  -c file          configurationfile (default: " TNTNET_CONF ")\n"
             "  -d               enable all debug output (ignoring properties-file)\n";
      throw std::invalid_argument(msg.str());
    }

    config.load(conf);

    if (debug)
    {
      log_init_debug();
      log_warn("Debugmode");
    }
    else
    {
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

    minthreads = config.getValue<unsigned>("MinThreads", 5);
    maxthreads = config.getValue<unsigned>("MaxThreads", 10);
    threadstartdelay = config.getValue<unsigned>("ThreadStartDelay", 0);
    worker::setMinThreads(minthreads);
    queue.setCapacity(config.getValue<unsigned>("QueueSize", 100));

    tntconfig::config_entries_type configSetEnv;
    config.getConfigValues("SetEnv", configSetEnv);
    for (tntconfig::config_entries_type::const_iterator it = configSetEnv.begin();
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
  }

  void tntnet::setGroup() const
  {
    tntconfig::params_type group = config.getConfigValue("Group");
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

  void tntnet::setDir(const char* def) const
  {
    std::string dir = config.getValue("Dir", def);
    if (!dir.empty() && chdir(dir.c_str()) == -1)
      throw std::runtime_error(
        std::string("error in chdir(): ")
          + strerror(errno));

    std::string chrootdir = config.getValue("Chroot");
    if (!chrootdir.empty() && chroot(chrootdir.c_str()) == -1)
      throw std::runtime_error(
        std::string("error in chroot(): ")
          + strerror(errno));
  }

  void tntnet::setUser() const
  {
    tntconfig::params_type user = config.getConfigValue("User");
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

  void tntnet::mkDaemon() const
  {
    log_info("start daemon-mode");

    int pid = fork();
    if (pid > 0)
    {
      // parent
      exit(0);
    }
    else if (pid < 0)
      throw std::runtime_error(
        std::string("error in fork(): ") + strerror(errno));

    // child

    // setsid
    if (setsid() == -1)
      throw std::runtime_error(
        std::string("error in setsid(): ")
          + strerror(errno));

    // setpgrp
    /*
    if (geteuid() == 0 && setpgrp() == -1)
      throw std::runtime_error(
        std::string("error in setpgrp(): ")
          + strerror(errno));
      */
  }

  void tntnet::closeStdHandles() const
  {
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

  static inline bool isTrue(char ch)
  {
    return ch == '1'
        || ch == 't' || ch == 'T'
        || ch == 'y' || ch == 'Y'
        || ch == 'j' || ch == 'J';
  }

  static inline bool isTrue(const std::string& s)
  {
    return s.size() > 0 && isTrue(s[0]);
  }

  int tntnet::run()
  {
    std::string daemon = config.getValue("Daemon");
    if (!debug && isTrue(daemon))
    {
      mkDaemon();

      // close stdin, stdout and stderr
      std::string noclosestd = config.getValue("NoCloseStdout");
      if (!isTrue(noclosestd))
        closeStdHandles();
      else
        log_debug("not closing stdout");

      std::string nomonitor = config.getValue("NoMonitor");
      if (isTrue(nomonitor))
      {
        log_debug("start worker-process without monitor");
        writePidfile(getpid());
        setDir("/");
        workerProcess();
      }
      else
      {
        do
        {
          // fork workerprocess
          int pid = fork();
          if (pid < 0)
            throw std::runtime_error(
              std::string("error in forking workerprocess: ")
                + strerror(errno));

          if (pid == 0)
          {
            // workerprocess
            setDir("/");
            workerProcess();
            return -1;
          }
          else
          {
            // write child-pid
            writePidfile(pid);
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
      setDir("");
      workerProcess();
    }

    return 0;
  }

  void tntnet::writePidfile(int pid)
  {
    pidFileName = config.getValue("PidFile", TNTNET_PID);

    log_debug("pidfile=" << pidFileName);

    if (!pidFileName.empty())
    {
      std::ofstream pidfile(pidFileName.c_str());
      if (!pidfile)
        throw std::runtime_error("unable to open pid-file " + pidFileName);
      pidfile << pid;
    }
  }

  void tntnet::monitorProcess(int workerPid)
  {
    if (chdir("/") == -1)
      throw std::runtime_error(
        std::string("error in chdir(): ")
          + strerror(errno));

    // close stdin, stdout and stderr
    closeStdHandles();

    int status;
    waitpid(workerPid, &status, 0);

    if (WIFSIGNALED(status))
    {
      // SIGTERM means normal exit
      if (WTERMSIG(status) == SIGTERM)
      {
        log_info("child terminated normally");
        stop = true;
      }
      else
      {
        log_info("child terminated with signal "
          << WTERMSIG(status) << " - restart child");
      }
    }
    else
    {
      log_info("child exited with exitcode " << WEXITSTATUS(status));
    }

    if (unlink(pidFileName.c_str()) != 0)
      log_warn("failed to remove pidfile \"" << pidFileName << "\" error " << errno);
  }

  void tntnet::workerProcess()
  {
    dispatcher dispatcher;
    configureDispatcher(dispatcher, config);

    // create listener-threads
    tntconfig::config_entries_type configListen;
    config.getConfigValues("Listen", configListen);

    if (configListen.empty())
    {
      log_warn("no listeners defined - using 0.0.0.0:80");
      cxxtools::Thread* s = new tnt::listener("0.0.0.0", 80, queue);
      listeners.insert(s);
    }
    else
    {
      for (tntconfig::config_entries_type::const_iterator it = configListen.begin();
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
        cxxtools::Thread* s = new tnt::listener(ip, port, queue);
        listeners.insert(s);
      }
    }

#ifdef USE_SSL
    // create ssl-listener-threads
    std::string defaultCertificateFile = config.getValue("SslCertificate");
    std::string defaultCertificateKey = config.getValue("SslKey");
    configListen.clear();
    config.getConfigValues("SslListen", configListen);

    for (tntconfig::config_entries_type::const_iterator it = configListen.begin();
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
        it->params.size() >= 4 ? it->params[3]
                               : defaultCertificateKey;

      if (certificateFile.empty())
        throw std::runtime_error("Ssl-certificate not configured");

      std::string ip(it->params[0]);
      log_debug("create ssl-listener ip=" << ip << " port=" << port);
      cxxtools::Thread* s = new ssllistener(certificateFile.c_str(),
          certificateKey.c_str(), ip, port, queue);
      listeners.insert(s);
    }
#endif // USE_SSL

    // change group and user
    setGroup();
    setUser();

    // configure worker (static)
    {
      tntconfig::config_entries_type compPath;
      config.getConfigValues("CompPath", compPath);
      for (tntconfig::config_entries_type::const_iterator it = compPath.begin();
           it != compPath.end(); ++it)
      {
        if (it->params.size() > 0)
          worker::addSearchPath(it->params[0]);
      }
    }

    // configure http
    httpMessage::setMaxRequestSize(config.getValue("MaxRequestSize", static_cast<unsigned>(0)));
    job::setSocketReadTimeout(config.getValue("SocketReadTimeout", static_cast<unsigned>(200)));
    job::setSocketWriteTimeout(config.getValue("SocketWriteTimeout", static_cast<unsigned>(10000)));
    job::setKeepAliveTimeout(config.getValue("KeepAliveTimeout", static_cast<unsigned>(15000)));
    job::setKeepAliveMax(config.getValue("KeepAliveMax", static_cast<unsigned>(100)));
    job::setSocketBufferSize(config.getValue("BufferSize", static_cast<unsigned>(16384)));

    // launch listener-threads
    log_info("create " << listeners.size() << " listener threads");
    for (listeners_type::iterator it = listeners.begin();
         it != listeners.end(); ++it)
      (*it)->Create();

    // create worker-threads
    log_info("create " << minthreads << " worker threads");
    for (unsigned i = 0; i < minthreads; ++i)
    {
      log_debug("create worker " << i);
      worker* s = new worker(queue, dispatcher, pollerthread, config);
      s->Create();
    }

    // create poller-thread
    log_debug("start poller thread");
    pollerthread.Create();

    log_debug("start cleaner thread");
    cxxtools::FunctionThread<void ()> cleaner_thread(worker::CleanerThread);
    cleaner_thread.Create();

    cxxtools::Mutex mutex;
    while (1)
    {
      {
        cxxtools::MutexLock lock(mutex);
        queue.noWaitThreads.Wait(lock);
      }

      if (worker::getCountThreads() < maxthreads)
      {
        log_info("create workerthread");
        worker* s = new worker(queue, dispatcher, pollerthread, config);
        s->Create();
      }
      else
        log_warn("max worker-threadcount " << maxthreads << " reached");

      if (threadstartdelay > 0)
        usleep(threadstartdelay);
    }

    // join-loop
    while (!listeners.empty())
    {
      listeners_type::value_type s = *listeners.begin();
      listeners.erase(s);
      s->Join();
      delete s;
    }
  }

  void tntnet::shutdown()
  {
    if (!pidFileName.empty())
      unlink(pidFileName.c_str());

    stop = true;

    // eigentlich nicht richtig, aber momentan die einzige Möglichkeit:
    exit(0);
  }
}

////////////////////////////////////////////////////////////////////////
// main
//

int main(int argc, char* argv[])
{
  signal(SIGPIPE, SIG_IGN);
  std::ios::sync_with_stdio(false);
  //setlocale(LC_ALL, "");

  try
  {
    tnt::tntnet app(argc, argv);
    return app.run();
  }
  catch(const std::exception& e)
  {
    log_fatal(e.what());
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
