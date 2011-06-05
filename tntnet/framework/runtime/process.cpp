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

  void sigEnd(int)
  {
    signal(SIGTERM, sigEnd);
    if (theProcess)
      theProcess->shutdown();
  }

  void sigReload(int)
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
    if (!::chroot(dir.c_str()) == -1)
      throw cxxtools::SystemError("chroot");
  }

  class PidFile
  {
      std::string pidFileName;
    public:
      PidFile(const std::string& pidFileName, pid_t pid);
      ~PidFile();

      void releasePidFile()  { pidFileName.clear(); }
  };

  PidFile::PidFile(const std::string& pidFileName_, pid_t pid)
    : pidFileName(pidFileName_)
  {
    if (pidFileName.empty())
      return;

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
          throw cxxtools::SystemError("getcwd");
      }
      pidFileName = std::string(cwd) + '/' + pidFileName;
      log_debug("pidfile=" << pidFileName);
    }

    std::ofstream pidfile(pidFileName.c_str());
    if (!pidfile)
      throw std::runtime_error("unable to open pid-file " + pidFileName);
    pidfile << pid;
    if (!pidfile)
      throw std::runtime_error("error writing to pid-file " + pidFileName);
  }

  PidFile::~PidFile()
  {
    if (!pidFileName.empty())
      ::unlink(pidFileName.c_str());
  }

  void closeStdHandles(const std::string& errorLog)
  {
    if (::freopen("/dev/null", "r", stdin) == 0)
      throw cxxtools::SystemError("freopen(stdin)");

    if (::freopen("/dev/null", "w", stdout) == 0)
      throw cxxtools::SystemError("freopen(stdout)");

    if (::freopen(errorLog.empty() ? "/dev/null" : errorLog.c_str(), "w", stderr) == 0)
      throw cxxtools::SystemError("freopen(stderr)");
  }

}

namespace tnt
{
  Process::Process(bool daemon_)
    : daemon(daemon_)
  {
    theProcess = this;
  }

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
        closeStdHandles(errorLog);

        exitRestart = false;
        log_debug("do work");
        doWork();

        // normal shutdown
        if (exitRestart)
          log_debug("restart");
        else
        {
          log_debug("signal shutdown");
          monitorPipe.write('s');
        }
        return;
      }

      // monitor-process

      log_debug("write pid " << fork.getPid() << " to \"" << pidfile << '"');
      PidFile p(pidfile, fork.getPid());

      if (first)
      {
        log_debug("close standard-handles");
        closeStdHandles(errorLog);
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

    if (!group.empty())
    {
      log_debug("set group to \"" << group << '"');
      ::setGroup(group);
    }

    if (!user.empty())
    {
      log_debug("set user to \"" << user << '"');
      ::setUser(user);
    }

    if (!dir.empty())
    {
      log_debug("set dir to \"" << dir << '"');
      ::setDir(dir);
    }

    if (!rootdir.empty())
    {
      log_debug("change root to \"" << rootdir << '"');
      ::setRootdir(rootdir);
    }

    signal(SIGTERM, sigEnd);
    signal(SIGINT, sigEnd);
    signal(SIGHUP, sigReload);
  }

  void Process::run()
  {
    if (daemon)
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
  {
    doShutdown();
  }

  void Process::restart()
  {
    exitRestart = true;
    doShutdown();
  }
}
