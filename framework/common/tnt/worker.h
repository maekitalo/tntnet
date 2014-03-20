/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#ifndef TNT_WORKER_H
#define TNT_WORKER_H

#include <string>
#include <cxxtools/thread.h>
#include <cxxtools/mutex.h>
#include <tnt/comploader.h>
#include <tnt/tntnet.h>
#include <tnt/scope.h>
#include <tnt/threadcontext.h>

namespace tnt
{
  class HttpRequest;
  class HttpReply;

  class Worker : public cxxtools::DetachedThread, private ThreadContext
  {
    private:
      typedef std::set<Worker*> workers_type;

      static cxxtools::Mutex _mutex;
      Tntnet& _application;
      static Comploader _comploader;

      Scope _threadScope;
      pthread_t _threadId;
      const char* _state;
      time_t _lastWaitTime;

      static workers_type _workers;

      bool processRequest(HttpRequest& request, std::iostream& socket, unsigned keepAliveCount);
      void logRequest(const HttpRequest& request, const HttpReply& reply, unsigned httpReturn);
      void healthCheck(time_t currentTime);

      // thread context methods
      void touch();       // wake watchdog timer
      Scope& getScope();

    public:
      explicit Worker(Tntnet& app);

      virtual void run();

      void dispatch(HttpRequest& request, HttpReply& reply);

      static void timer();

      static workers_type::size_type getCountThreads();

      static Comploader& getComponentLoader()
        { return _comploader; }
  };
}

#endif // TNT_WORKER_H

