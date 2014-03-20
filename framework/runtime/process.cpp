/*
 * Copyright (C) 2006 Tommi Maekitalo
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
#include "tnt/tntconfig.h"
#include <cxxtools/systemerror.h>
#include <cxxtools/posix/fork.h>
#include <pwd.h>
#include <grp.h>
#include <cxxtools/log.h>
#include <vector>
#include <fstream>
#include <errno.h>
#include <signal.h>

log_define("tntnet.process")

namespace
{
  tnt::Process* theProcess = 0;

  extern "C" void sigEnd(int)
  {
    signal(SIGTERM, sigEnd);
    if (theProcess)
      theProcess->shutdown();
  }

  extern "C" void sigReload(int)
  {
    signal(SIGHUP, sigReload);
    if (theProcess)
      theProcess->restart();
  }

  void setGroup(const std::string& group)
  {
    struct group * gr = ::getgrnam(group.c_str());
    if (gr == 0)
      throw std::runtime_error("unknown group " + group);

    log_debug("change group to " << group << '(' << gr->gr_gid << ')');

    int ret = ::setgid(gr->gr_gid);
    if (ret != 0)
      throw cxxtools::SystemError("setgid");
  }

  void setUser(const std::string& user)
  {
    struct passwd * pw = getpwnam(user.c_str());
    if (pw == 0)
      throw std::runtime_error("unknown user " + user);

    log_debug("change user to " << user << '(' << pw->pw_uid << ')');

    int ret = ::setuid(pw->pw_uid);
    if (ret != 0)
      throw cxxtools::SystemError("getuid");
  }

  void setDir(const std::string& dir)
  {
    log_debug("chdir(" << dir << ')');
    if (::chdir(dir.c_str()) == -1)
      throw cxxtools::SystemError("chdir");
  }

  void setRootdir(const std::string& dir)
  {
    if (::chroot(dir.c_str()) == -1)
      throw cxxtools::SystemError("chroot");
  }

  class PidFile
  {
    private:
      std::string _pidFileName;
    public:
      PidFile(const std::string& pidFileName, pid_t pid);
      ~PidFile();

      void releasePidFile() { _pidFileName.clear(); }
  };

  PidFile::PidFile(const std::string& pidFileName, pid_t pid)
    : _pidFileName(pidFileName)
  {
    if (_pidFileName.empty())
      return;

    if (_pidFileName[0] != '/')
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
          throw cxxtools::SystemError("getcwd");
      }
      _pidFileName = std::string(cwd) + '/' + _pidFileName;
      log_debug("pidfile=" << _pidFileName);
    }

    std::ofstream pidfile(_pidFileName.c_str());
    if (pidfile.fail())
      throw std::runtime_error("unable to open pid-file " + _pidFileName);

    pidfile << pid;

    if (pidfile.fail())
      throw std::runtime_error("error writing to pid-file " + _pidFileName);
  }

  PidFile::~PidFile()
  {
    if (!_pidFileName.empty())
      ::unlink(_pidFileName.c_str());
  }

  void closeStdHandles(const std::string& errorLog = std::string())
  {
    if (::freopen("/dev/null", "r", stdin) == 0)
      throw cxxtools::SystemError("freopen(stdin)");

    if (::freopen("/dev/null", "w", stdout) == 0)
      throw cxxtools::SystemError("freopen(stdout)");

    if (!errorLog.empty())
    {
      if (::freopen(errorLog.c_str(), "a+", stderr) == 0)
        throw cxxtools::SystemError("freopen(stderr)");
    }
  }
}

namespace tnt
{
  Process::Process()
    { theProcess = this; }

  Process::~Process()
  {
    if (theProcess == this)
      theProcess = 0;
  }

  void Process::runMonitor(cxxtools::posix::Pipe& mainPipe)
  {
    log_debug("run monitor");

    // setsid
    if (setsid() == -1)
      throw cxxtools::SystemError("setsid");

    bool first = true;

    while (true)
    {
      cxxtools::posix::Pipe monitorPipe;

      cxxtools::posix::Fork fork;

      if (fork.child())
      {
        // worker-process

        log_debug("close read-fd of monitor-pipe");
        monitorPipe.closeReadFd();

        initWorker();
        if (first)
        {
          log_debug("signal initialization ready");
          mainPipe.write('1');
          log_debug("close write-fd of main-pipe");
          mainPipe.closeWriteFd();
        }

        log_debug("close standard-handles");
        closeStdHandles(tnt::TntConfig::it().errorLog);

        _exitRestart = false;
        log_debug("do work");
        doWork();

        // normal shutdown
        if (_exitRestart)
          log_debug("restart");
        else
        {
          log_debug("signal shutdown");
          try
          {
            monitorPipe.write('s');
          }
          catch (const std::exception& e)
          {
            log_debug("ingore exception from monitor pipe: " << e.what());
          }
        }
        return;
      }

      // monitor-process

      log_debug("write pid " << fork.getPid() << " to \"" << tnt::TntConfig::it().pidfile << '"');
      PidFile p(tnt::TntConfig::it().pidfile, fork.getPid());

      if (first)
      {
        log_debug("close standard-handles");
        closeStdHandles();
        first = false;
      }

      monitorPipe.closeWriteFd();
      try
      {
        log_debug("monitor child");
        char dummy;
        size_t c = monitorPipe.read(&dummy, 1);
        if (c > 0)
        {
          log_debug("child terminated normally");
          return;
        }
        log_debug("nothing read from monitor-pipe - restart child");
      }
      catch (const cxxtools::SystemError&)
      {
        log_debug("child exited without notification");
      }

      log_debug("wait for child-termination");
      fork.wait();

      ::sleep(1);
    }
  }

  void Process::initWorker()
  {
    log_debug("init worker");

    log_debug("onInit");
    onInit();

    if (!tnt::TntConfig::it().group.empty())
    {
      log_debug("set group to \"" << tnt::TntConfig::it().group << '"');
      ::setGroup(tnt::TntConfig::it().group);
    }

    if (!tnt::TntConfig::it().user.empty())
    {
      log_debug("set user to \"" << tnt::TntConfig::it().user << '"');
      ::setUser(tnt::TntConfig::it().user);
    }

    if (!tnt::TntConfig::it().dir.empty())
    {
      log_debug("set dir to \"" << tnt::TntConfig::it().dir << '"');
      ::setDir(tnt::TntConfig::it().dir);
    }

    if (!tnt::TntConfig::it().chrootdir.empty())
    {
      log_debug("change root to \"" << tnt::TntConfig::it().chrootdir << '"');
      ::setRootdir(tnt::TntConfig::it().chrootdir);
    }

    signal(SIGTERM, sigEnd);
    signal(SIGINT, sigEnd);
    signal(SIGHUP, sigReload);
  }

  void Process::run()
  {
    if (tnt::TntConfig::it().daemon)
    {
      log_debug("run daemon-mode");

      // We receive the writing-end of the notify pipe.
      // After successful initialization we need to write a byte to this fd.
      cxxtools::posix::Pipe mainPipe;
      cxxtools::posix::Fork fork;
      if (fork.parent())
      {
        log_debug("close write-fd of main-pipe");
        mainPipe.closeWriteFd();

        log_debug("wait for child to initialize");
        mainPipe.read();

        log_debug("child initialized");

        fork.setNowait();
      }
      else
      {
        log_debug("close read-fd of main-pipe");
        mainPipe.closeReadFd();

        runMonitor(mainPipe);
      }
    }
    else
    {
      log_debug("run");
      initWorker();

      log_debug("do work");
      doWork();
    }
  }

  void Process::shutdown()
    { doShutdown(); }

  void Process::restart()
  {
    _exitRestart = true;
    doShutdown();
  }
}

