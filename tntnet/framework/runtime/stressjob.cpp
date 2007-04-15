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

#include <tnt/stressjob.h>
#include <cxxtools/log.h>

log_define("tntnet.testjob");

namespace tnt
{
  StressHttpClientStreambuf::DummyHttpClientStreambuf(const std::string& url)
  {
    request = "GET " + url + " HTTP/1.0\r\nUser-agent: dummy\r\n\r\n";
    char* p = const_cast<char*>(request.data());
    setg(p, p, p + request.size());
  }

  StressHttpClientStreambuf::int_type DummyHttpClientStreambuf::overflow(int_type c)
  {
    return 0;
  }

  StressHttpClientStreambuf::int_type DummyHttpClientStreambuf::underflow()
  {
    return traits_type::eof();
  }

  int StressHttpClientStreambuf::sync()
  {
    return 0;
  }

  cxxtools::Mutex StressJob::mutex;
  unsigned StressJob::count = 0;

  StressJob::DummyJob(Jobqueue& queue_, const std::string& url_)
    : client(url_),
      init(false),
      queue(queue_),
      url(url_)
  {
    cxxtools::MutexLock lock(mutex);
    if (++count % 10000 == 0)
      log_info(count << " dummy-jobs processed");
  }

  std::iostream& StressJob::getStream()
  {
    if (!init)
    {
      if (count % 100000 == 0)
        usleep(1000000);
      queue.put(new StressJob(queue, url));
      init = true;
    }
    return client;
  }

  int StressJob::getFd() const
  {
    return 0;
  }

  void StressJob::setRead()
  {
  }

  void StressJob::setWrite()
  {
  }

}
