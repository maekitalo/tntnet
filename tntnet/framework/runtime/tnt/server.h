////////////////////////////////////////////////////////////////////////
// server.h
//

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cxxtools/tcpstream.h>
#include <cxxtools/thread.h>
#include <tnt/comploader.h>
#include "tnt/logfwd.h"

namespace tnt
{
  class httpRequest;
  class httpReply;
  class dispatcher;
  class jobqueue;

  class server : public Thread
  {
      static Mutex mutex;
      static unsigned nextThreadNumber;

      jobqueue& queue;

      const dispatcher& ourdispatcher;
      comploader mycomploader;

      unsigned threadNumber;

      typedef std::set<server*> servers_type;
      static servers_type servers;

      static unsigned compLifetime;

      log_declare_class();

    public:
      server(jobqueue& queue, const dispatcher& dispatcher,
        comploader::load_library_listener* libconfigurator);

      virtual void Run();

      void Dispatch(httpRequest& request, httpReply& reply);
      void cleanup(unsigned seconds)
      { mycomploader.cleanup(seconds); }
      static void addSearchPath(const std::string& path)
      { comploader::addSearchPath(path); }

      static void CleanerThread();
      static void setCompLifetime(unsigned sec)
      { compLifetime = sec; }

    private:
      void executeQuery(const std::string& header);
  };
}

#endif // SERVER_H

