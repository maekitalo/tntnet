/*
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

#ifndef TNT_STRESSJOB_H
#define TNT_STRESSJOB_H

#include <tnt/job.h>
#include <string>
#include <cxxtools/mutex.h>
#include <iostream>

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

  class StressJob : public Job
  {
      StressHttpClient client;
      bool init;
      Jobqueue& queue;
      std::string url;
      static cxxtools::Mutex mutex;
      static unsigned count;

    public:
      StressJob(Jobqueue& queue_, const std::string& url_);
      std::iostream& getStream();
      int getFd() const;
      void setRead();
      void setWrite();
  };

}

#endif //  TNT_STRESSJOB_H
