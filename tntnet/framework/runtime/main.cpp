/* main.cpp
 * Copyright (C) 2007 Tommi Maekitalo
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


#include <iostream>
#include "tnt/tntnet.h"
#include "tnt/tntconfig.h"
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <sys/stat.h>
#include "tnt/process.h"

log_define("tntnet.main")

namespace tnt
{
  class TntnetProcess : public Process
  {
      tnt::Tntconfig config;
      tnt::Tntnet tntnet;
      bool logall;

      void initializeLogging();

    protected:
      virtual void onInit();
      virtual void doWork();
      virtual void doShutdown();

    public:
      TntnetProcess(int& argc, char* argv[]);
  };

  TntnetProcess::TntnetProcess(int& argc, char* argv[])
    : logall(cxxtools::Arg<bool>(argc, argv, "--logall"))
  {
    std::string configFile;

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

    config.load(configFile.c_str());

    if (logall)
      initializeLogging();

    setDaemon(config.getBoolValue("Daemon", false));
    setPidFile(config.getValue("PidFile", TNTNET_PID));
  }

  void TntnetProcess::initializeLogging()
  {
    std::string pf = config.getValue("PropertyFile");

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

  void TntnetProcess::onInit()
  {
    tntnet.init(config);
  }

  void TntnetProcess::doWork()
  {
    if (!logall)
      initializeLogging();

    tntnet.run();
  }

  void TntnetProcess::doShutdown()
  {
    tnt::Tntnet::shutdown();
  }
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    tnt::TntnetProcess app(argc, argv);

    if (argc != 1)
    {
      std::cout << PACKAGE_STRING "\n\n" <<
                   "usage: " << argv[0] << " configurationfile (default: " TNTNET_CONF ")"
                << std::endl;
      return -1;
    }

    app.run();
  }
  catch(const std::exception& e)
  {
    log_fatal(e.what());
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
