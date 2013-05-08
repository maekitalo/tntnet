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

#include <tnt/stressjob.h>
#include <tnt/tntnet.h>
#include <cxxtools/log.h>

log_define("tntnet.testjob")

namespace tnt
{
  StressHttpClientStreambuf::StressHttpClientStreambuf(const std::string& url)
  {
    request = "GET " + url + " HTTP/1.0\r\nUser-agent: dummy\r\n\r\n";
    char* p = const_cast<char*>(request.data());
    setg(p, p, p + request.size());
  }

  StressHttpClientStreambuf::int_type StressHttpClientStreambuf::overflow(int_type c)
  {
    return 0;
  }

  StressHttpClientStreambuf::int_type StressHttpClientStreambuf::underflow()
  {
    return traits_type::eof();
  }

  int StressHttpClientStreambuf::sync()
  {
    return 0;
  }

  cxxtools::Mutex StressJob::mutex;
  unsigned StressJob::count = 0;

  StressJob::StressJob(Tntnet& app_, const std::string& url_)
    : Job(app_, this),
      client(url_),
      init(false),
      app(app_),
      url(url_)
  {
    cxxtools::MutexLock lock(mutex);
    if (++count % 10000 == 0)
      log_info(count << " dummy-jobs processed");
  }

  std::string StressJob::getPeerIp() const
  {
      return std::string();
  }

  std::string StressJob::getServerIp() const
  {
      return std::string();
  }

  bool StressJob::isSsl() const
  {
      return false;
  }

  std::iostream& StressJob::getStream()
  {
    if (!init)
    {
      app.getQueue().put(new StressJob(app, url));
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
