/*
 * Copyright (C) 2007 Tommi Maekitalo
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


#include "tnt/process.h"
#include "tnt/tntnet.h"
#include "tnt/cmd.h"
#include "tnt/tntconfig.h"

#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/xml/xmldeserializer.h>
#include <cxxtools/jsondeserializer.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <stdexcept>
#include <set>

#include <glob.h>
#include <sstream>

#include "config.h"

#ifndef TNTNET_CONF
# define TNTNET_CONF "/etc/tntnet.xml"
#endif

#ifndef TNTNET_PID
# define TNTNET_PID "/var/run/tntnet.pid"
#endif


log_define("tntnet.main")

namespace tnt
{
  namespace
  {
    class Glob
    {
      private:
        glob_t _gl;
        unsigned _n;

        // disable copy and assignment
        Glob(const Glob&) { }
        Glob& operator= (const Glob&) { return *this; }

      public:
        explicit Glob(const std::string& pattern, int flags = 0);
        ~Glob();

        const char* current() const
          { return _gl.gl_pathv ? _gl.gl_pathv[_n] : 0; }

        const char* next()
        {
          if (_gl.gl_pathv && _gl.gl_pathv[_n])
            ++_n;
          return current();
        }
    };

    Glob::Glob(const std::string& pattern, int flags)
      : _n(0)
    {
      int ret = ::glob(pattern.c_str(), flags, 0, &_gl);
      if (ret == GLOB_NOMATCH)
      {
        _gl.gl_pathv = 0;
      }
      else if (ret != 0)
      {
        std::ostringstream msg;
        msg << "failed to process glob pattern <" << pattern << "> errorcode " << ret;
        throw std::runtime_error(msg.str());
      }
    }

    Glob::~Glob() { globfree(&_gl); }

    template <typename Deserializer>
    void processConfigFile(const std::string& configFile, std::set<std::string>& filesProcessed)
    {
      TntConfig& config = TntConfig::it();

      std::ifstream in(configFile.c_str());
      if (!in)
        throw std::runtime_error("failed to open configuration file \"" + configFile + '"');

      Deserializer deserializer(in);
      deserializer.deserialize(config);

      in.close();

      filesProcessed.insert(configFile);

      for (std::vector<std::string>::size_type n = 0; n < config.includes.size(); ++n)
      {
        for (Glob glob(config.includes[n]); glob.current(); glob.next())
        {
          std::string configFile = glob.current();
          if (filesProcessed.find(configFile) == filesProcessed.end())
            processConfigFile<Deserializer>(glob.current(), filesProcessed);
        }
      }
    }
  }

  class TntnetProcess : public Process
  {
      tnt::Tntnet _tntnet;
      bool _logall;

      void initializeLogging();

    protected:
      virtual void onInit();
      virtual void doWork();
      virtual void doShutdown();

    public:
      TntnetProcess(int& argc, char* argv[]);
  };

  TntnetProcess::TntnetProcess(int& argc, char* argv[])
    : _logall(cxxtools::Arg<bool>(argc, argv, "--logall"))
  {
    std::string configFile;

    cxxtools::Arg<bool> jsonConfig(argc, argv, 'j');

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
          configFile = "tntnet.xml";
        else
          configFile = TNTNET_CONF;  // take default
      }
    }

    std::set<std::string> filesProcessed;
    if (jsonConfig)
      processConfigFile<cxxtools::JsonDeserializer>(configFile, filesProcessed);
    else
      processConfigFile<cxxtools::xml::XmlDeserializer>(configFile, filesProcessed);

    if (_logall)
      initializeLogging();
  }

  void TntnetProcess::initializeLogging()
  {
    const cxxtools::SerializationInfo* psi = TntConfig::it().config.findMember("logging");
    if (psi)
      log_init(*psi);
  }

  void TntnetProcess::onInit()
    { _tntnet.init(TntConfig::it()); }

  void TntnetProcess::doWork()
  {
    if (!_logall)
      initializeLogging();

    _tntnet.run();
  }

  void TntnetProcess::doShutdown()
    { tnt::Tntnet::shutdown(); }
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    cxxtools::Arg<bool> version(argc, argv, "--version");
    if (version)
    {
      std::cout << PACKAGE_STRING "\n" << std::flush;
      return 0;
    }

    cxxtools::Arg<bool> help1(argc, argv, 'h');
    cxxtools::Arg<bool> help2(argc, argv, '?');

    if (help1 || help2)
    {
      std::cout << "usage: " << argv[0] << " configurationfile (default: " TNTNET_CONF ")"
                << std::endl;
      return 0;
    }

    cxxtools::Arg<bool> cmd(argc, argv, 'C');
    if (cmd)
    {
      log_init("tntnet.xml");

      tnt::Cmd cmdapp(std::cout);

      tnt::QueryParams queryParams;

      while (true)
      {
        cxxtools::Arg<std::string> param(argc, argv, 'q');
        if (!param.isSet())
          break;
        queryParams.parse_url(param);
      }

      for (int a = 1; a < argc; ++a)
      {
        log_info("calling component <" << argv[a] << '>');
        cmdapp.call(tnt::Compident(argv[a]), queryParams);
      }
    }
    else
    {
      tnt::TntnetProcess app(argc, argv);

      app.run();
    }
  }
  catch(const std::exception& e)
  {
    log_fatal(e.what());
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

