/* tnt/worker.h
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNT_WORKER_H
#define TNT_WORKER_H

#include <string>
#include <cxxtools/tcpstream.h>
#include <cxxtools/thread.h>
#include <cxxtools/pool.h>
#include <tnt/comploader.h>
#include <tnt/tntnet.h>

namespace tnt
{
  class HttpRequest;
  class HttpReply;

  class ComploaderCreator
  {
      static const Tntconfig* config;

    public:
      static void setConfig(const Tntconfig& config_)
        { config = &config_; }

      Comploader* operator() ()
      {
        return new Comploader(*config);
      }
  };

  class Worker : public cxxtools::DetachedThread
  {
      static cxxtools::Mutex mutex;
      static unsigned nextThreadNumber;

      Tntnet& application;

      typedef cxxtools::Pool<Comploader, ComploaderCreator> ComploaderPoolType;
      static ComploaderPoolType comploaderPool;

      ComploaderPoolType::objectptr_type comploaderObject;
      Comploader& comploader;

      unsigned threadId;
      const char* state;
      time_t lastWaitTime;

      typedef std::set<Worker*> workers_type;
      static workers_type workers;

      static unsigned compLifetime;
      static unsigned maxRequestTime;
      static unsigned reportStateTime;
      static time_t nextReportStateTime;
      static unsigned minThreads;

      bool processRequest(HttpRequest& request, std::iostream& socket,
        unsigned keepAliveCount);
      void healthCheck(time_t currentTime);

      ~Worker();

    public:
      Worker(Tntnet& app);

      virtual void run();

      void dispatch(HttpRequest& request, HttpReply& reply);
      void cleanup(unsigned seconds)
        { comploader.cleanup(seconds); }
      static void addSearchPath(const std::string& path)
        { Comploader::addSearchPath(path); }

      static void timer();

      static void setCompLifetime(unsigned sec)    { compLifetime = sec; }
      static unsigned getCompLifetime()            { return compLifetime; }

      /// Sets a hard limit for request-time.
      /// When the time is exceeded, this process exits.
      static void setMaxRequestTime(unsigned sec)  { maxRequestTime = sec; }
      static unsigned getMaxRequestTime()          { return maxRequestTime; }

      static void setReportStateTime(unsigned sec) { reportStateTime = sec; }
      static unsigned getReportStateTime()         { return reportStateTime; }


      static workers_type::size_type getCountThreads();
      static void setMinThreads(unsigned n)
      { minThreads = n; }
  };
}

#endif // TNT_WORKER_H

