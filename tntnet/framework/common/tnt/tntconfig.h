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

#ifndef TNT_TNTCONFIG_H
#define TNT_TNTCONFIG_H

#include <string>
#include <vector>
#include <map>
#include <cxxtools/serializationinfo.h>

namespace tnt
{
  static const int SSL_ALL = 0;
  static const int SSL_NO  = 1;
  static const int SSL_YES = 2;

  struct TntConfig
  {

    struct Mapping
    {
      std::string target;
      std::string url;
      std::string vhost;
      std::string method;
      std::string pathinfo;
      int ssl;

      typedef std::map<std::string, std::string> ArgsType;

      ArgsType args;
    };

    struct Listener
    {
      std::string ip;
      unsigned short port;
    };

    struct SslListener : public Listener
    {
      std::string certificate;
      std::string key;
    };

    typedef std::vector<Mapping> MappingsType;
    typedef std::vector<Listener> ListenersType;
    typedef std::vector<SslListener> SslListenersType;
    typedef std::vector<std::string> CompPathType;
    typedef std::map<std::string, std::string> EnvironmentType;

    MappingsType mappings;
    ListenersType listeners;
    SslListenersType ssllisteners;
    unsigned maxRequestSize;
    unsigned maxRequestTime;
    std::string user;
    std::string group;
    std::string dir;
    std::string chrootdir;
    std::string pidfile;
    bool daemon;
    unsigned minThreads;
    unsigned maxThreads;
    unsigned long threadStartDelay;
    unsigned queueSize;
    CompPathType compPath;
    unsigned socketBufferSize;
    unsigned socketReadTimeout;
    unsigned socketWriteTimeout;
    unsigned keepAliveTimeout;
    unsigned keepAliveMax;
    unsigned sessionTimeout;
    unsigned listenBacklog;
    unsigned listenRetry;
    bool enableCompression;
    unsigned minCompressSize;
    std::string mimeDb;
    unsigned maxUrlMapCache;
    std::string defaultContentType;
    std::string accessLog;
    std::string errorLog;
    unsigned backgroundTasks;
    unsigned timerSleep;
    cxxtools::SerializationInfo config;
    EnvironmentType environment;
    std::string documentRoot;
    std::vector<std::string> includes;

    TntConfig();

    bool hasValue(const std::string& key) const
    { return config.findMember(key) != 0; }

    static TntConfig& it();
  };

  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Mapping& mapping);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Listener& listener);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::SslListener& ssllistener);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig& config);
}

#endif // TNT_TNTCONFIG_H

