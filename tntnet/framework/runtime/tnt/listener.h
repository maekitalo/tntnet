/* tnt/listener.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef TNT_LISTENER_H
#define TNT_LISTENER_H

#include <cxxtools/thread.h>
#include "tnt/job.h"
#include "tnt/ssl.h"

namespace tnt
{
  class listener : public cxxtools::Thread
  {
      cxxtools::tcp::Server server;
      jobqueue& queue;

    public:
      listener(const std::string& ipaddr, unsigned short int port, jobqueue& q);
      virtual void Run();
  };

  class ssllistener : public cxxtools::Thread
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

