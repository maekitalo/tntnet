////////////////////////////////////////////////////////////////////////
// listener.cpp
//

#include "tnt/listener.h"
#include "tnt/ssl.h"
#include "tnt/log.h"
#include "tnt/tntnet.h"

namespace tnt
{
  listener::listener(const std::string& ipaddr, unsigned short int port, jobqueue& q)
    : server(ipaddr, port),
      queue(q)
  {
    log_debug("Listen to " << ipaddr << " port " << port);
  }

  void listener::Run()
  {
    // accept-loop
    log_debug("enter accept-loop");
    while (!tntnet::shouldStop())
    {
      try
      {
        tcpjob* j = new tcpjob;
        jobqueue::job_ptr p(j);
        j->Accept(server);
        queue.put(p);
      }
      catch (const std::exception& e)
      {
        log_error("error in accept-loop: " << e.what());
      }
    }
  }

  ssllistener::ssllistener(const char* certificateFile,
      const char* keyFile,
      const std::string& ipaddr, unsigned short int port,
      jobqueue& q)
    : server(certificateFile, keyFile),
      queue(q)
  {
    log_debug("Listen to " << ipaddr << " port " << port << " (ssl)");
    server.Listen(ipaddr.c_str(), port);
  }

  void ssllistener::Run()
  {
    // accept-loop
    log_debug("enter accept-loop (ssl)");
    while (!tntnet::shouldStop())
    {
      try
      {
        ssl_tcpjob* j = new ssl_tcpjob;
        jobqueue::job_ptr p(j);
        j->Accept(server);
        queue.put(p);
      }
      catch (const std::exception& e)
      {
        log_error("error in accept-loop: " << e.what());
      }
    }
  }
}
