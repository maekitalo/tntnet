/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef TNT_STRESSJOB_H
#define TNT_STRESSJOB_H

#include <tnt/job.h>
#include <string>
#include <cxxtools/mutex.h>
#include <iostream>
#include <tnt/socketif.h>

/**
 * This defines a job, which makes a local request to stresstest tntnet
 * without network. The job regenerates itself automatically so that tntnet
 * runs as fast as possible.
 *
 * To use it, tntnet has to put a single stressjob into the queue on
 * initialization.
 */

namespace tnt
{
  class StressHttpClientStreambuf : public std::streambuf
  {
      std::string request;

    public:
      StressHttpClientStreambuf(const std::string& url);
      int_type overflow(int_type c);
      int_type underflow();
      int sync();
  };

  class StressHttpClient : public std::iostream
  {
      StressHttpClientStreambuf streambuf;

    public:
      explicit StressHttpClient(const std::string& url)
        : std::iostream(0),
          streambuf(url)
          { rdbuf(&streambuf); }
  };

  class StressJob : public Job, private SocketIf
  {
      StressHttpClient client;
      bool init;
      Tntnet& app;
      std::string url;
      static cxxtools::Mutex mutex;
      static unsigned count;

      virtual std::string getPeerIp() const;
      virtual std::string getServerIp() const;
      virtual bool isSsl() const;

    public:
      StressJob(Tntnet& app_, const std::string& url_);
      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };

}

#endif //  TNT_STRESSJOB_H
