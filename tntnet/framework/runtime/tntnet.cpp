////////////////////////////////////////////////////////////////////////
// tntnet.cpp
//

#include "tnt/server.h"
#include "tnt/tntnet.h"
#include "tnt/dispatcher.h"
#include "tnt/listener.h"
#include "tnt/log.h"
#include "tnt/loginit.h"

#include <iostream>
#include <fstream>
#include <cxxtools/tcpstream.h>
#include <unistd.h>
#include <log4cplus/configurator.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
//#include <locale.h>

#ifndef CONFIG_DIR
# define CONFIG_DIR "/etc/tntnet/"
#endif

#ifndef TNTNET_CONF
# define TNTNET_CONF CONFIG_DIR "tntnet.conf"
#endif

#ifndef TNTNET_PROPERTIES
# define TNTNET_PROPERTIES CONFIG_DIR "tntnet.properties"
#endif

namespace
{
  ////////////////////////////////////////////////////////////////////////
  // libconfigurator
  //
  class libconfigurator : public tnt::comploader::load_library_listener
  {
      typedef void (*config_function_type)(const std::string& name,
        const std::vector<std::string>& values);

      const tnt::tntconfig& config;
      void configure(config_function_type config_function) const;

    public:
      libconfigurator(const tnt::tntconfig& c)
        : config(c)
        { }
      void onLoadLibrary(tnt::component_library& lib);
      void onCreateComponent(tnt::component_library& lib,
        const tnt::compident& ci, tnt::component& comp);
  };

  void libconfigurator::configure(config_function_type config_function) const
  {
    const tnt::tntconfig::config_values_type& values =
    config.getConfigValues();

    tnt::tntconfig::config_values_type::const_iterator vi;
    for (vi = values.begin(); vi != values.end(); ++vi)
    {
      const tnt::tntconfig::config_entry_type& v = *vi;
      config_function(v.key, v.values);
    }
  }

  void libconfigurator::onLoadLibrary(tnt::component_library& lib)
  {
    dl::symbol config_symbol;
    try
    {
      config_symbol = lib.sym("config");
      config_function_type config_function =
        (config_function_type)config_symbol.getSym();
      configure(config_function);

    }
    catch(const dl::symbol_not_found&)
    {
    }
  }

  void libconfigurator::onCreateComponent(tnt::component_library& lib,
    const tnt::compident& ci, tnt::component& comp)
  {
    dl::symbol config_symbol;
    try
    {
      config_symbol = lib.sym(("config_" + ci.compname).c_str());
      config_function_type config_function =
        (config_function_type)config_symbol.getSym();
      configure(config_function);
    }
    catch(const dl::symbol_not_found&)
    {
    }
  }

  void configureDispatcher(tnt::dispatcher& dis, const tnt::tntconfig& config)
  {
    typedef tnt::dispatcher::compident_type compident_type;

    const tnt::tntconfig::config_values_type& values = config.getConfigValues();

    tnt::tntconfig::config_values_type::const_iterator vi;
    for (vi = values.begin(); vi != values.end(); ++vi)
    {
      const tnt::tntconfig::config_entry_type& v = *vi;
      const tnt::tntconfig::config_value_type& args = v.values;
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
  static void sig_stopprogram(int)
  {
    tntnet::shutdown();
  }

  ////////////////////////////////////////////////////////////////////////
  // tntnet
  //
  bool tntnet::stop = false;
  std::string tntnet::pidFileName;
  log_define_class(tntnet, "tntnet.tntnet");

  tntnet::tntnet(int argc, char* argv[])
    : arg_ip(argc, argv, 'i', "0.0.0.0"),
      arg_port(argc, argv, 'p', 8000),
      arg_numthreads(argc, argv, 't', 5),
      conf(argc, argv, 'c', TNTNET_CONF),
      propertyfilename(argc, argv, 'P'),
      debug(argc, argv, 'd'),
      arg_lifetime(argc, argv, 'C', 60),
      numthreads(5),
      lifetime(60),
      cleaner_thread(*this, &tntnet::Clean)
  {
    if (argc != 1)
    {
      std::ostringstream msg;
      msg << "Aufruf: " << argv[0] << " {Optionen}\n\n"
             "  -i ip-adresse    IP-Adresse, auf der der Server hören soll\n"
             "                   (default: 0.0.0.0)\n"
             "  -p portnummer    Portnummer, auf der der Server hören soll\n"
             "                   (default: 8000)\n"
             "  -t anzahl        Anzahl der zu startenden Threads (default: 5)\n"
             "  -c Datei         Konfigurationsdatei (default: " TNTNET_CONF ")\n"
             "  -C Zeit          Lebenszeit unbenutzter Objekte (default: 60)\n";
      throw std::invalid_argument(msg.str());
    }

    config.load(conf);

    if (debug)
    {
      log_init_debug();
      log_warn("Debugmodus aktiv");
    }
    else
    {
      std::string pf;
      if (propertyfilename.isSet())
        pf = propertyfilename.getValue();
      else
        pf = config.getSingleValue("PropertyFile");

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

    ip = arg_ip;
    port = arg_port;

    if (!arg_ip.isSet())
    {
      tntconfig::config_value_type v = config.getConfigValue("Listen");
      if (v.size() == 1)
        ip = *v.begin();
    }

    if (!arg_port.isSet())
    {
      tntconfig::config_value_type v = config.getConfigValue("Port");
      if (v.size() == 1)
      {
        std::istringstream in(*v.begin());
        in >> port;
      }
    }

    numthreads = arg_numthreads;
    if (!arg_numthreads.isSet())
    {
      tntconfig::config_value_type v = config.getConfigValue("Threads");
      if (v.size() == 1)
      {
        std::istringstream in(*v.begin());
        in >> numthreads;
      }
    }

    lifetime = arg_lifetime;
    if (!arg_lifetime.isSet())
    {
      tntconfig::config_value_type v = config.getConfigValue("Lifetime");
      if (v.size() >= 1)
      {
        std::istringstream in(*v.begin());
        in >> lifetime;
      }
    }

    tntconfig::config_values_type configSetEnv;
    config.getConfigValues("SetEnv", configSetEnv);
    for (tntconfig::config_values_type::const_iterator it = configSetEnv.begin();
         it != configSetEnv.end(); ++it)
    {
      if (it->values.size() >= 2)
      {
        log_debug("setenv " << it->values[0] << "=\"" << it->values[1] << '"');
        ::setenv(it->values[0].c_str(), it->values[1].c_str(), 1);
      }
    }
  }

  void tntnet::setGroup() const
  {
    tntconfig::config_value_type group = config.getConfigValue("Group");
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

  void tntnet::setUser() const
  {
    tntconfig::config_value_type user = config.getConfigValue("User");
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
    return ch == '1' || ch == 't' || ch == 'T' || ch == 'j' || ch == 'J';
  }

  static inline bool isTrue(const std::string& s)
  {
    return isTrue(s[0]);
  }

  int tntnet::run()
  {
    std::string daemon = config.getSingleValue("Daemon");
    if (!debug && isTrue(daemon))
    {
      mkDaemon();

      // close stdin, stdout and stderr
      std::string noclosestd = config.getSingleValue("NoCloseStdout");
      if (!isTrue(noclosestd))
        closeStdHandles();
      else
        log_debug("not closing stdout");

      std::string nomonitor = config.getSingleValue("NoMonitor");
      if (isTrue(nomonitor))
      {
        log_debug("start worker-process without monitor");
        if (chdir("/") == -1)
          throw std::runtime_error(
            std::string("error in chdir(): ")
              + strerror(errno));
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
            if (chdir("/") == -1)
              throw std::runtime_error(
                std::string("error in chdir(): ")
                  + strerror(errno));

            workerProcess();
            return -1;
          }
          else
          {
            monitorProcess(pid);
            if (!stop)
              sleep(1);
          }

        } while (!stop);
      }
    }
    else
      workerProcess();

    return 0;
  }

  void tntnet::monitorProcess(int workerPid)
  {
    // write child-pid
    pidFileName = config.getSingleValue("PidFile");
    if (!pidFileName.empty())
    {
      std::ofstream pidfile(pidFileName.c_str());
      if (!pidfile)
        throw std::runtime_error(
          "unable to open PidFile " + pidFileName);
      pidfile << workerPid;
    }

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
    unlink(pidFileName.c_str());
  }

  void tntnet::workerProcess()
  {
    dispatcher dispatcher;
    configureDispatcher(dispatcher, config);

    libconfigurator myconfigurator(config);

    // create listener-threads
    tntconfig::config_values_type configListen;
    config.getConfigValues("Listen", configListen);

    if (configListen.empty())
    {
      log_warn("no listeners defined - using 0.0.0.0:80");
      Thread* s = new tnt::listener("0.0.0.0", 80, queue);
      listeners.insert(s);
    }
    else
    {
      for (tntconfig::config_values_type::const_iterator it = configListen.begin();
           it != configListen.end(); ++it)
      {
        if (it->values.empty())
          throw std::runtime_error("empty Listen-entry");

        unsigned short int port = 80;
        if (it->values.size() >= 2)
        {
          std::istringstream p(it->values[1]);
          p >> port;
          if (!p)
          {
            std::ostringstream msg;
            msg << "invalid port " << it->values[1];
            throw std::runtime_error(msg.str());
          }
        }

        std::string ip(it->values[0]);
        log_debug("create listener ip=" << ip << " port=" << port);
        Thread* s = new tnt::listener(ip, port, queue);
        listeners.insert(s);
      }
    }

    // create ssl-listener-threads
    std::string certificateFile = config.getSingleValue("SslCertificate");
    std::string certificateKey = config.getSingleValue("SslKey", certificateFile);
    configListen.clear();
    config.getConfigValues("SslListen", configListen);

    if (!configListen.empty() && certificateFile.empty())
      throw std::runtime_error("SslCertificate not configured");

    for (tntconfig::config_values_type::const_iterator it = configListen.begin();
         it != configListen.end(); ++it)
    {
      if (it->values.empty())
        throw std::runtime_error("empty SslListen-entry");

      unsigned short int port = 443;
      if (it->values.size() >= 2)
      {
        std::istringstream p(it->values[1]);
        p >> port;
        if (!p)
        {
          std::ostringstream msg;
          msg << "invalid port " << it->values[1];
          throw std::runtime_error(msg.str());
        }
      }

      std::string ip(it->values[0]);
      log_debug("create ssl-listener ip=" << ip << " port=" << port);
      Thread* s = new ssllistener(certificateFile.c_str(), certificateKey.c_str(),
          ip, port, queue);
      listeners.insert(s);
    }

    // change group and user
    setGroup();
    setUser();

    // configure server (static)
    {
      tntconfig::config_values_type compPath;
      config.getConfigValues("CompPath", compPath);
      for (tntconfig::config_values_type::const_iterator it = compPath.begin();
           it != compPath.end(); ++it)
      {
        if (it->values.size() > 0)
          server::addSearchPath(it->values[0]);
      }
    }

    // create server-threads
    log_info("create " << numthreads << " server threads");
    for (unsigned i = 0; i < numthreads; ++i)
    {
      server* s = new server(queue, dispatcher, &myconfigurator);
      servers.insert(s);
      log_debug("create server " << i);
    }

    // launch listener-threads
    log_info("create " << listeners.size() << " listener threads");
    for (listeners_type::iterator it = listeners.begin();
         it != listeners.end(); ++it)
      (*it)->Create();

    // launch server-threads
    for (servers_type::iterator it = servers.begin();
         it != servers.end(); ++it)
      (*it)->Create();

    log_debug("start cleaner thread");
    cleaner_thread.Create();

    // TODO: load-monitoring/dynamic thread-creation

    // join-loop
    while (!servers.empty())
    {
      servers_type::value_type s = *servers.begin();
      servers.erase(s);
      s->Join();
      delete s;
    }

    while (!listeners.empty())
    {
      listeners_type::value_type s = *listeners.begin();
      listeners.erase(s);
      s->Join();
      delete s;
    }
  }

  void tntnet::Clean()
  {
    while (!stop)
    {
      unsigned count = 10;
      while (count > 0 && !stop)
      {
        --count;
        sleep(1);
      }

      if (!stop)
      {
        log_debug("cleanup");
        for (servers_type::iterator it = servers.begin();
             it != servers.end(); ++it)
        {
          server* s = *it;
          s->cleanup(lifetime);
        }
      }
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

#define NO_NAMESPACE
log_define_namespace(NO_NAMESPACE, "main")

int main(int argc, char* argv[])
{
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
  }
}
