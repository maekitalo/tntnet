/* tnt/tcpjob.h
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_TCPJOB_H
#define TNT_TCPJOB_H

#include <tnt/job.h>
#include <cxxtools/tcpstream.h>
#include <tnt/ssl.h>

namespace tnt
{
  class Tcpjob : public Job
  {
      cxxtools::net::iostream socket;
      const cxxtools::net::Server& listener;
      Jobqueue& queue;

      void accept();

    public:
      Tcpjob(Tntnet& app, const cxxtools::net::Server& listener_, Jobqueue& queue_)
        : Job(app),
          socket(getSocketBufferSize(), getSocketReadTimeout()),
          listener(listener_),
          queue(queue_)
        { }

      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };

#ifdef USE_SSL
  class SslTcpjob : public Job
  {
      ssl_iostream socket;
      const SslServer& listener;
      Jobqueue& queue;

      void accept();

    public:
      SslTcpjob(Tntnet& app, const SslServer& listener_, Jobqueue& queue_)
        : Job(app),
          socket(getSocketBufferSize(), getSocketReadTimeout()),
          listener(listener_),
          queue(queue_)
        { }

      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };
#endif // USE_SSL

}

#endif // TNT_TCPJOB_H
