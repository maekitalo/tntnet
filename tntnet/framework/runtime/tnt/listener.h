////////////////////////////////////////////////////////////////////////
// tnt/listener.h
//

#ifndef TNT_LISTENER_H
#define TNT_LISTENER_H

#include <cxxtools/thread.h>
#include "tnt/job.h"
#include "tnt/ssl.h"

namespace tnt
{
  class listener : public Thread
  {
      tcp::Server server;
      jobqueue& queue;

    public:
      listener(const std::string& ipaddr, unsigned short int port, jobqueue& q);
      virtual void Run();
  };

  class ssllistener : public Thread
  {
      SslServer server;
      jobqueue& queue;

    public:
      ssllistener(const char* certificateFile, const char* keyFile,
          const std::string& ipaddr, unsigned short int port, jobqueue& q);
      virtual void Run();
  };
}

#endif // TNT_LISTENER_H

