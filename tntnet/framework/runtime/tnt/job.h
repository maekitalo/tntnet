////////////////////////////////////////////////////////////////////////
// tnt/job.h
//

#ifndef TNT_JOB_H
#define TNT_JOB_H

#include <boost/shared_ptr.hpp>
#include <deque>
#include <cxxtools/thread.h>
#include <cxxtools/tcpstream.h>
#include "tnt/ssl.h"

/**
// in tntnet (mainthread):
jobqueue queue;
void mainloop()
{
  while (1)
  {
    jobqueue::ptr_type j = new tcpjob();
    j->Accept(poller.get());
    queue.put(j);
  }
}

// in server (workerthread):
void server::Run()
{
  while (1)
  {
    jobqueue::ptr_type j = queue.get();
    std::iostream& socket = j->getStream();
    processRequest(socket);
  }
}
*/

namespace tnt
{
  /** job - one per request */
  class job
  {
    public:
      virtual ~job() { }
      virtual std::iostream& getStream() = 0;
      virtual const struct sockaddr_in& getPeeraddr_in() const = 0;
      virtual const struct sockaddr_in& getServeraddr_in() const = 0;
      virtual bool isSsl() const = 0;
  };

  class tcpjob : public job
  {
      tcp::iostream socket;
      struct sockaddr_in sockaddr_in;

    public:
      tcpjob()
        : socket(1024, 60000)
        { }

      void Accept(const tcp::Server& listener);

      std::iostream& getStream();
      const struct sockaddr_in& getPeeraddr_in() const;
      const struct sockaddr_in& getServeraddr_in() const;
      bool isSsl() const     { return false; }
  };

  class ssl_tcpjob : public job
  {
      ssl_iostream socket;
      struct sockaddr_in sockaddr_in;

    public:
      ssl_tcpjob()
        : socket(1024, 60000)
        { }

      void Accept(const SslServer& listener);

      std::iostream& getStream();
      const struct sockaddr_in& getPeeraddr_in() const;
      const struct sockaddr_in& getServeraddr_in() const;
      bool isSsl() const     { return true; }
  };

  /** jobqueue - one per process */
  class jobqueue
  {
    public:
      typedef boost::shared_ptr<job> job_ptr;

    private:
      std::deque<job_ptr> jobs;
      Condition notEmpty;

    public:
      void put(job_ptr j);
      job_ptr get();
  };
}

#endif // TNT_JOB_H

