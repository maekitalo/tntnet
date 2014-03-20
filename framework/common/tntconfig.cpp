/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include <tnt/tntconfig.h>
#include <cxxtools/log.h>
#include "config.h"

log_define("tntnet.tntconfig")

namespace tnt
{
  namespace
  {
    struct VirtualHost
    {
      std::string hostname;
      TntConfig::MappingsType mappings;
    };

    void operator>>= (const cxxtools::SerializationInfo& si, VirtualHost& virtualHost)
    {
      si.getMember("hostname") >>= virtualHost.hostname;
      si.getMember("mappings") >>= virtualHost.mappings;
      for (TntConfig::MappingsType::iterator it = virtualHost.mappings.begin(); it != virtualHost.mappings.end(); ++it)
      {
        if (!it->vhost.empty())
          log_warn("vhost entry not empty in mapping \"" << it->url << "\" in virtual host \"" << virtualHost.hostname);
        it->vhost = virtualHost.hostname;
      }
    }

  }

  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Mapping& mapping)
  {
    si.getMember("target") >>= mapping.target;
    si.getMember("url", mapping.url);
    si.getMember("vhost", mapping.vhost);
    si.getMember("method", mapping.method);
    si.getMember("pathinfo", mapping.pathinfo);
    bool ssl;
    if (si.getMember("ssl", ssl))
      mapping.ssl = ssl ? SSL_YES : SSL_NO;
    else
      mapping.ssl = SSL_ALL;

    const cxxtools::SerializationInfo* args = si.findMember("args");
    if (args)
    {
      for (cxxtools::SerializationInfo::ConstIterator it = args->begin(); it != args->end(); ++it)
      {
        std::string value;
        it->getValue(value);
        mapping.args[it->name()] = value;
      }
    }
  }

  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Listener& listener)
  {
    si.getMember("ip", listener.ip);

    if (!si.getMember("port", listener.port))
      listener.port = 80;
  }

  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::SslListener& ssllistener)
  {
    si.getMember("ip", ssllistener.ip);

    if (!si.getMember("port", ssllistener.port))
      ssllistener.port = 443;

    si.getMember("certificate") >>= ssllistener.certificate;

    if (!si.getMember("key", ssllistener.key))
      ssllistener.key = ssllistener.certificate;
  }

  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig& config)
  {
    std::vector<VirtualHost> virtualHosts;
    if (si.getMember("virtualhosts", virtualHosts))
    {
      for (std::vector<VirtualHost>::const_iterator it = virtualHosts.begin(); it != virtualHosts.end(); ++it)
        config.mappings.insert(config.mappings.end(), it->mappings.begin(), it->mappings.end());
    }

    TntConfig::MappingsType mappings;
    if (si.getMember("mappings", mappings))
      config.mappings.insert(config.mappings.end(), mappings.begin(), mappings.end());

    TntConfig::ListenersType& listeners = config.listeners;
    TntConfig::SslListenersType& ssllisteners = config.ssllisteners;

    const cxxtools::SerializationInfo& lit = si.getMember("listeners");
    for (cxxtools::SerializationInfo::ConstIterator it = lit.begin(); it != lit.end(); ++it)
    {
      if (it->findMember("certificate") != 0)
      {
        ssllisteners.resize(ssllisteners.size() + 1);
        *it >>= ssllisteners.back();
      }
      else
      {
        listeners.resize(listeners.size() + 1);
        *it >>= listeners.back();
      }
    }

    if (config.listeners.empty() && config.ssllisteners.empty())
    {
      config.listeners.resize(1);
      config.listeners.back().port = 80;
    }

    si.getMember("maxRequestSize", config.maxRequestSize);
    si.getMember("maxRequestTime", config.maxRequestTime);
    si.getMember("user", config.user);
    si.getMember("group", config.group);
    si.getMember("dir", config.dir);
    si.getMember("chrootdir", config.chrootdir);
    si.getMember("pidfile", config.pidfile);
    si.getMember("daemon", config.daemon);
    si.getMember("minThreads", config.minThreads);
    si.getMember("maxThreads", config.maxThreads);
    si.getMember("threadStartDelay", config.threadStartDelay);
    si.getMember("queueSize", config.queueSize);
    si.getMember("compPath", config.compPath);
    si.getMember("socketBufferSize", config.socketBufferSize);
    si.getMember("socketReadTimeout", config.socketReadTimeout);
    si.getMember("socketWriteTimeout", config.socketWriteTimeout);
    si.getMember("keepAliveTimeout", config.keepAliveTimeout);
    si.getMember("keepAliveMax", config.keepAliveMax);
    si.getMember("sessionTimeout", config.sessionTimeout);
    si.getMember("listenBacklog", config.listenBacklog);
    si.getMember("listenRetry", config.listenRetry);
    si.getMember("enableCompression", config.enableCompression);
    si.getMember("minCompressSize", config.minCompressSize);
    si.getMember("mimeDb", config.mimeDb);
    si.getMember("maxUrlMapCache", config.maxUrlMapCache);
    si.getMember("defaultContentType", config.defaultContentType);
    si.getMember("accessLog", config.accessLog);
    si.getMember("errorLog", config.errorLog);
    si.getMember("timerSleep", config.timerSleep);
    si.getMember("documentRoot", config.documentRoot);
    si.getMember("server", config.server);
    si.getMember("includes", config.includes);

    config.config = si;

    const cxxtools::SerializationInfo* p = si.findMember("environment");
    if (p)
    {
      for (cxxtools::SerializationInfo::ConstIterator it = p->begin(); it != p->end(); ++it)
      {
        std::string value;
        it->getValue(value);
        config.environment[it->name()] = value;
      }
    }

  }

  TntConfig::TntConfig()
    : maxRequestSize(0),
      maxRequestTime(600),
      daemon(false),
      minThreads(5),
      maxThreads(100),
      threadStartDelay(10),
      queueSize(1000),
      socketBufferSize(16384),
      socketReadTimeout(10),
      socketWriteTimeout(10000),
      keepAliveTimeout(15000),
      keepAliveMax(1000),
      sessionTimeout(300),
      listenBacklog(512),
      listenRetry(5),
      enableCompression(true),
      minCompressSize(1024),
      mimeDb("/etc/mime.types"),
      maxUrlMapCache(8192),
      defaultContentType("text/html; charset=UTF-8"),
      timerSleep(10),
      server("Tntnet/" VERSION)
    { }

  TntConfig& TntConfig::it()
  {
    static TntConfig theConfig;
    return theConfig;
  }
}

