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
    // TODO: doc
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

    // TODO: doc
    struct Listener
    {
      std::string ip;
      unsigned short port;
    };

    // TODO: doc
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

    /** A list of mappings (see Mapping)

        default: none
     */
    MappingsType mappings;

    /** A list of listeners (see Listener)

        default: none
     */
    ListenersType listeners;

    /** A list of ssl listeners (see SslListener)

        default: none
     */
    SslListenersType ssllisteners;

    /** The maximal size of a request

        If a larger request is sent, it is ignored. This limit prevents
        denial-of-service attacks through long requests. Bear in mind
        though that this also limits the possible size of file-uploads.
        
        default: 0 (no limit)
     */
    unsigned maxRequestSize;

    /** The maximal time (in seconds) /the worker thread may use to answer a http request?/

        If /answering the request/ takes longer, the whole tntnet server is restarted,
        which means dropping all active connections. Therefore the time set here should
        be at least as long as the most time-intensive request may take.

        default: 600 seconds
     */
    unsigned maxRequestTime;

    /** The unix user id of the user tntnet should switch to when executed as root

        If this string is unset (empty) or tntnet is not executed as root, the user under
        which tntnet operates does not change. The switching is done through setuid().

        default: (none)
     */
    std::string user;

    /** The unix group id of the group tntnet should switch to when executed as root

        If this option is omitted (empty string) or tntnet is not executed as root, the group
        under which tntnet operates does not change. The switching is done through setgid().

        default: (none)
     */
    std::string group;

    /** The working dir in which tntnet should operate

        If this option is omitted (empty string), tntnet
        keeps the working dir from which it is executed.

        default: (none)
     */
    std::string dir;

    /** A directory into which tntnet should switch with chroot

        This locks tntnet into that directory through chroot(), which means
        files outside it are no more visible and the directory becomes the
        root directory ("/") for file access operations.

        If this option is omitted (empty string), no chroot is performed.

        default: (none)
     */
    std::string chrootdir;

    /** A file into which the process id (pid) should be written

        The pid that is written into the file will be the one of the monitor process as long as
        it exists. If it is deactivated, the pid of the worker thread is written into the file.

        If this option is omitted (empty string), no file is written.

        default: (none)
     */
    std::string pidfile;

    /** Whether to run tntnet as daemon

        If this is set to true, tntnet forks itself and terminates the parent process.

        default: false
     */
    bool daemon;

    // TODO: Add link to documentation of thread model
    /** The minimal number of worker threads

        default: 5
     */
    unsigned minThreads;

     /** The maximal number of worker threads

        default: 100
     */
    unsigned maxThreads;

    /** Time (in milliseconds) to wait before spawning a new worker thread

        When many new worker threads are needed, this delay prevents high
        processor load that may be caused by spawning them all as soon as possible.

        default: 10 milliseconds
     */
    unsigned long threadStartDelay;

    /** Limit for the size of the request queue

        This is a limit for the number of request that can
        simultaneously be queued while waiting to be processed.

        default: 1000
     */
    unsigned queueSize;

    // TODO: What happens if this is omitted (empty) as it is default?
    /** The paths where tntnet searches for the compiled webapps

        This is only relevant if the webapps are compiled into
        shared libraries instead of a standalone executable.

        default: (none)
     */
    CompPathType compPath;

    // TODO: Why is this named socketBufferSize here and bufferSize in tntnet.xml?
    //       And what is this for???
    /** Size (in bytes) of data bundled for a system call

        default: 16384
     */
    unsigned socketBufferSize;

    // TODO: I didn't fully understand the documentation on this,
    //       a complete explanation should be added here.
    /** The timeout (in milliseconds) for reading data from a client

        default: 10 milliseconds
     */
    unsigned socketReadTimeout;

    /** The timeout (in milliseconds) for writing data to a client

        default: 10000 milliseconds
     */
    unsigned socketWriteTimeout;

    /** The timeout (in milliseconds) for keeping a TCP connection alive

        Normally, keep-alive connections are used, which means the TCP connection between server
        and client stays for multiple requests. After this timeout, the connection is closed.

        default: 15000
     */
    unsigned keepAliveTimeout;

    /** Maximal amount of requests per TCP connection

        default: 1000
     */
    unsigned keepAliveMax;

    /** The amount of time (in seconds) after which a session times out

        default: 300
     */
    unsigned sessionTimeout;

    /** The amount of connection requests to be queued by the system

        Before tntnet accepts an incoming TCP connection request, it is stored in the
        OS'es backlog. This option determines how many connections the OS should store
        in the backlog. When this limit is reached, new connection requests are rejected.

        default: 512
     */
    unsigned listenBacklog;

    /** The amount of retries to listen for incoming connections

        If listening on a local interface fails, tntnet will retry the specified amount of times.

        default: 5
     */
    unsigned listenRetry;

    /** Whether to enable gzip compression for data sent to the client

        Unless this is set to false, tntnet compresses all data using gzip if
        @li the data can be compressed by more than 10% and
        @li the client sends the flag "Accept-Encoding" in the http request header

        default: true
     */
    bool enableCompression;

    // TODO: Size in bytes correct?
    /** Minimal size (in bytes) of http reply body to be compressed

        If an http body is smaller than this, it will not be compressed.

        default: 1024
     */
    unsigned minCompressSize;

    /** Filename of the mime database

        The mime database is used to look up the mime-type that is sent in the http header.

        default: "/etc/mime.types"
     */
    std::string mimeDb;

    /** The maximal amount of entries in the url map cache

        tntnet caches the results of component lookups because going through the whole
        mapping table and possibly executing many regular expressions is time expensive.
        After this limit for the amout of entries is reached, the cache is cleared.

        default: 8192
     */
    unsigned maxUrlMapCache;

    // TODO: This is not enough information, why is this option not documented for tntnet.xml?
    /** The default mime-type for the http header

        default: "text/html; charset=UTF-8"
     */
    std::string defaultContentType;

    // TODO: Does specifying this option remove the access log on stdout?
    /** Logfile for server requests

        The format of the logging entries is compatible with logging tools for http servers.

        If this option is omitted (empty string), no logfile is written.
     */
    std::string accessLog;

    /** Logfile for errors

        This option redirects stderr to the specified file.
        It is crucial to set this if you run tntnet as daemon.

        If this option is omitted (empty string), no logfile is written.
     */
    std::string errorLog;

    // TODO: doc or flag internal
    /** 

        default: 10
     */
    unsigned timerSleep;

    // TODO: doc or flag internal
    cxxtools::SerializationInfo config;

    // TODO: doc or flag internal
    EnvironmentType environment;

    // TODO: doc or flag internal
    std::string documentRoot;

    // TODO: doc or flag internal
    /** 

        default: true
     */
    bool hasServer;

    // TODO: doc or flag internal
    std::string server;

    // TODO: doc or flag internal
    std::vector<std::string> includes;

    /// Create a TntConfig object with default configuration
    TntConfig();

    // TODO: doc or flag internal
    bool hasValue(const std::string& key) const
    { return config.findMember(key) != 0; }

    // TODO: doc or flag internal
    static TntConfig& it();
  };

  // TODO: doc or flag internal
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Mapping& mapping);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Listener& listener);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::SslListener& ssllistener);
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig& config);
}

#endif // TNT_TNTCONFIG_H

