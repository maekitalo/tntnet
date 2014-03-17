/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#ifndef TNT_CONFIGURATOR_H
#define TNT_CONFIGURATOR_H

#include <tnt/tntnet.h>
#include <tnt/worker.h>
#include <tnt/sessionscope.h>
#include <tnt/httpreply.h>
#include <tnt/tntconfig.h>

namespace tnt
{
  /** Helper class for easier configuration of tntnet when used as library.

      Configuration of tntnet is spread througout the system and it is a
      tendious task to search the right place, where to set some setting. This
      Class helps here and offers methods for all settings in one place. The
      methods dispatch the setting to the right place, so the user needs to
      know only this class, when he wants to set some settings.

      This is yet deprecated since all configuration is done in tnt::TntConfig.
   */
  class Configurator
  {
    private:
      Tntnet& tntnet;

    public:
      explicit Configurator(tnt::Tntnet& tntnet)
        : _tntnet(tntnet)
        { }

      /// Returns the minimum number of worker threads.
      unsigned getMinThreads() const
        { return TntConfig::it().minThreads; }
      /// Sets the minimum number of worker threads.
      void setMinThreads(unsigned n)
        { TntConfig::it().minThreads = n; }

      /// Returns the maximum number of worker threads.
      unsigned getMaxThreads() const
        { return TntConfig::it().maxThreads; }
      /// Sets the maximum number of worker threads.
      void setMaxThreads(unsigned n)
        { TntConfig::it().maxThreads = n; }

      /// Returns the time in seconds after which cleanup like checking sessiontimeout is done.
      unsigned getTimerSleep() const
        { return TntConfig::it().timerSleep; }
      /// Sets the time in seconds after which cleanup like checking sessiontimeout is done.
      void setTimerSleep(unsigned sec)
        { TntConfig::it().timerSleep = sec; }

      /// Returns the time in seconds between thread starts.
      unsigned getThreadStartDelay() const
        { return TntConfig::it().threadStartDelay; }
      /// Sets the time in seconds between thread starts.
      void setThreadStartDelay(unsigned sec)
        { TntConfig::it().threadStartDelay = sec; }

      /// Returns the maximum number of jobs waiting for processing.
      unsigned getQueueSize() const
        { return TntConfig::it().queueSize; }
      /// Sets the maximum number of jobs waiting for processing.
      void setQueueSize(unsigned n)
        { TntConfig::it().queueSize = n; }

      /// Returns the maximum request time, after which tntnet is automatically restarted in daemon mode.
      unsigned getMaxRequestTime() const
        { return TntConfig::it().maxRequestTime; }
      /// Sets the maximum request time, after which tntnet is automatically restarted in daemon mode.
      void setMaxRequestTime(unsigned sec)
        { TntConfig::it().maxRequestTime = sec; }

      /// Returns true, when http compression is used.
      bool getEnableCompression() const
        { return TntConfig::it().enableCompression; }
      /// enables or disables http compression.
      void setEnableCompression(bool sw = true)
        { TntConfig::it().enableCompression = sw; }

      /// Returns the time of inactivity in seconds after which a session is destroyed
      unsigned getSessionTimeout() const
        { return TntConfig::it().sessionTimeout; }
      /// Sets the time of inactivity in seconds after which a session is destroyed
      void setSessionTimeout(unsigned sec)
        { TntConfig::it().sessionTimeout = sec; }

      /// Returns the listen backlog parameter (see also listen(2)).
      int getListenBacklog() const
        { return TntConfig::it().listenBacklog; }
      /// Sets the listen backlog parameter (see also listen(2)).
      void setListenBacklog(int n)
        { TntConfig::it().listenBacklog = n; }

      /// Returns the number of retries, when a listen retried, when failing.
      unsigned getListenRetry() const
        { return TntConfig::it().listenRetry; }
      /// Sets the number of retries, when a listen retried, when failing.
      void setListenRetry(int n)
        { TntConfig::it().listenRetry = n; }

      /// Returns the maximum number of cached urlmappings.
      /// The cache stores results of the regular expressions used for defining
      /// mappings. Since the number of different urls used is normally quite
      /// limited, the cache reduces significantly the number of executed
      /// regular expressions.
      unsigned getMaxUrlMapCache() const
        { return TntConfig::it().maxUrlMapCache; }
      /// Sets the maximum number of cached urlmappings.
      void setMaxUrlMapCache(int n)
        { TntConfig::it().maxUrlMapCache = n; }

      /// Returns the maximum size of a request.
      /// Requestdata are collected in memory and therefore requests, which
      /// exceed the size of available memory may lead to a denial of service.
      size_t getMaxRequestSize() const
        { return TntConfig::it().maxRequestSize; }
      /// Sets the maximum size of a request.
      void setMaxRequestSize(size_t s)
        { TntConfig::it().maxRequestSize = s; }

      /// Returns the read timeout in millisecods after which the request is passed to the poller.
      /// Tntnet tries to keep a connection on the same thread to reduce
      /// context switches by waiting a short period if additional data
      /// arrives. After that period the request is passed to the poller,
      /// which waits for activity on the socket. The default value is
      /// 10 ms.
      unsigned getSocketReadTimeout() const
        { return TntConfig::it().socketReadTimeout; }
      /// Sets the timeout in millisecods after which the request is passed to the poller.
      void setSocketReadTimeout(unsigned ms)
        { TntConfig::it().socketReadTimeout = ms; }

      /// Returns the write timeout in millisecods after which the request is timed out.
      /// The default value is 10000 ms.
      unsigned getSocketWriteTimeout() const
        { return TntConfig::it().socketWriteTimeout; }
      /// Sets the write timeout in millisecods after which the request is timed out.
      void setSocketWriteTimeout(unsigned ms)
        { TntConfig::it().socketWriteTimeout = ms; }

      /// Returns the maximum number of requests handled over a single connection.
      /// The default value is 1000.
      unsigned getKeepAliveMax() const
        { return TntConfig::it().keepAliveMax; }
      /// Sets the maximum number of requests handled over a single connection.
      void setKeepAliveMax(unsigned ms)
        { TntConfig::it().keepAliveMax = ms; }

      /// Returns the size of the socket buffer.
      /// This specifies the maximum number of bytes after which the data sent
      /// or received over a socket is passed to the operating system.
      /// The default value is 16384 bytes (16 kBytes).
      unsigned getSocketBufferSize() const
        { return TntConfig::it().socketBufferSize; }
      /// Sets the size of the socket buffer.
      void setSocketBufferSize(unsigned ms)
        { TntConfig::it().socketBufferSize = ms; }

      /// Returns the minimum size of a request body for compression.
      /// Small requests are not worth compressing, so tntnet has a limit,
      /// to save cpu. The default value is 1024 bytes.
      unsigned getMinCompressSize() const
        { return TntConfig::it().minCompressSize; }
      /// Sets the minimum size of a request body for compression.
      void setMinCompressSize(unsigned s)
        { TntConfig::it().minCompressSize = s; }

      /// Returns the keep alive timeout in milliseconds.
      /// This specifies, how long a connection is kept for keep alive. A keep
      /// alive request binds (little) resources, so it is good to free it
      /// after some time of inactivity. The default value if 15000 ms.
      unsigned getKeepAliveTimeout() const
        { return TntConfig::it().keepAliveTimeout; }
      /// Sets the keep alive timeout in milliseconds.
      void setKeepAliveTimeout(unsigned s)
        { TntConfig::it().keepAliveTimeout = s; }

      /// Returns the default content type.
      /// The default content type is "text/html; charset=iso-8859-1".
      const std::string& getDefaultContentType() const
        { return TntConfig::it().defaultContentType; }
      /// Sets the default content type.
      void setDefaultContentType(const std::string& s)
        { TntConfig::it().defaultContentType = s; }

      void setAccessLog(const std::string& accessLog)
        { TntConfig::it().accessLog = accessLog; }
  };
}

#endif // TNT_CONFIGURATOR_H

