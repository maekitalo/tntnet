////////////////////////////////////////////////////////////////////////
// tntnet.h
//

#ifndef TNTNET_H
#define TNTNET_H

#include <cxxtools/arg.h>
#include "tnt/tntconfig.h"
#include "tnt/job.h"
#include <set>
#include "tnt/log.h"

namespace tnt
{
  class tntnet
  {
      arg<const char*> arg_ip;
      arg<unsigned short int> arg_port;
      arg<unsigned> arg_numthreads;
      arg<const char*> conf;
      tntconfig config;
      arg<const char*> propertyfilename;
      arg<bool> debug;
      arg<unsigned> arg_lifetime;

      unsigned numthreads;
      unsigned short int port;
      std::string ip;
      unsigned lifetime;

      jobqueue queue;

      static bool stop;
      typedef std::set<Thread*> listeners_type;
      listeners_type listeners;

      static std::string pidFileName;

      // helper methods
      void setUser() const;
      void setGroup() const;
      void mkDaemon() const;
      void closeStdHandles() const;

      // noncopyable
      tntnet(const tntnet&);
      tntnet& operator= (const tntnet&);

      void monitorProcess(int workerPid);
      void workerProcess();

      log_declare_class();

    public:
      tntnet(int argc, char* argv[]);
      int run();

      static void shutdown();
      static bool shouldStop()   { return stop; }
  };

}

#endif // TNTNET_H

