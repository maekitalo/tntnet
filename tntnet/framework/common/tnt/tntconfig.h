/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#ifndef TNT_TNTCONFIG_H
#define TNT_TNTCONFIG_H

#include <string>
#include <vector>
#include <map>

namespace tnt
{
  struct TntConfig
  {
    struct Mapping
    {
      std::string target;
      std::string url;
      std::string vhost;
      std::string method;
      std::string pathinfo;
      enum { SSL_ALL, SSL_NO, SSL_YES } ssl;

      typedef std::vector<std::string> ArgsType;

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
    typedef std::map<std::string, std::string> ConfigType;
    typedef std::map<std::string, std::string> EnvironmentType;

    MappingsType mappings;
    ListenersType listeners;
    SslListenersType ssllisteners;
    std::string logproperties;
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
    ConfigType config;
    EnvironmentType environment;
    std::string documentRoot;

    TntConfig();

    void load(const char* fname);

    std::string getConfigValue(const std::string& key, const std::string& def = std::string()) const;
    bool hasValue(const std::string& key) const
    { return config.find(key) != config.end(); }

    static TntConfig& it();
  };

}

#endif // TNT_TNTCONFIG_H

