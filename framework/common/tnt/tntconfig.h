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

  /** This structure represents the configuration of a tntnet server.

      On startup tntnet (the tntnet server executable) reads the configuration
      from tntnet.xml or tntnet.json into a static instance of TntConfig.
      This static configuration can be accessed using TntConfig::it().

      %TntConfig has many predefined configuration parameters but may contain
      additional custom attributes, which can be used in applications using the
      #config member. The easiest way to access additional configuration options
      is to add a <%config> section into the ecpp components where they are used.
      Variables specified there are automatically read from the static %TntConfig
      instance and deserialized into the specified data type.

      In standalone applications, the configuration file is not automatically
      read. It is up to the user to configure the application. Still this
      class can help setting up a server. Using Tntnet::init(), the user
      can pass the configuration to the application class.

      Since the class is also deserializable with cxxtools, it is easy to read
      the configuration from a file as the tntnet executable does.

      Example:
      @code
        // Instantiate an application object
        tnt::Tntnet app;

        // Create a configuration object
        tnt::TntConfig config;

        // Alternatively, the static instance can be used:
        // tnt::TntConfig& config = tnt::TntConfig::it();

        // Open the configfile
        std::ifstream tntnetXml("tntnet.xml");

        // Deserialize the content of tntnetXml and save it to config
        tntnetXml >> cxxtools::Xml(config);

        // Pass the configuration to the application object
        app.init(config);
      @endcode

      Note:
      This deserialization code only works when <cxxtools/xml.h> is included
      and you have cxxtools >= 2.3 installed. In previous cxxtools versions
      the deserialization has to be done like this:

      @code
        // Deserialize the content of tntnetXml and save it to config
        cxxtools::xml::XmlDeserializer deserializer(tntnetXml);
        deserializer.deserialize(config);
      @endcode
   */
  struct TntConfig
  {
    /// A mapping entry
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

    /// A listener entry
    struct Listener
    {
      std::string ip;
      unsigned short port;
    };

    /// An SSL listener entry
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
        Denial of Service attacks through long requests. Bear in mind
        though that this also limits the possible size of file uploads.

        default: 0 (no limit)
     */
    unsigned maxRequestSize;

    /** The maximal time (in seconds) a worker thread may use to answer an http request

        If answering the request takes longer, the whole tntnet server is restarted,
        which means dropping all active connections. Therefore the time set here should
        be at least as long as the most time-intensive request may take.

        default: 600 seconds
     */
    unsigned maxRequestTime;

    /** The unix user id of the user tntnet should switch to when executed as root

        If this string is unset (empty) or tntnet is not executed as root, the user under
        which tntnet operates does not change. The switching is done through setuid().

        default: (empty)
     */
    std::string user;

    /** The unix group id of the group tntnet should switch to when executed as root

        If this option is unset (empty) or tntnet is not executed as root, the group
        under which tntnet operates does not change. The switching is done through setgid().

        default: (empty)
     */
    std::string group;

    /** The working dir in which tntnet should operate

        If this option is unset (empty), tntnet keeps
        the working directory from which it is executed.

        default: (empty)
     */
    std::string dir;

    /** A directory into which tntnet should switch with chroot

        This locks tntnet into that directory through chroot(), which means
        files outside it are no more visible and the directory becomes the
        root directory ("/") for file access operations.

        If this option is unset (empty), no chroot is performed.

        default: (empty)
     */
    std::string chrootdir;

    /** A file to which the process id (pid) should be written

        The pid that is written into the file will be the one of the monitor process as long as
        it exists. If it is deactivated, the pid of the worker thread is written into the file.

        If this option is unset (empty), no file is written.

        default: (empty)
     */
    std::string pidfile;

    /** Whether to run tntnet as daemon

        If this is set to true, tntnet forks itself and terminates the parent process.

        default: false
     */
    bool daemon;

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

        The limit for the number of request that can
        simultaneously be queued while waiting to be processed.

        default: 1000
     */
    unsigned queueSize;

    /** The paths where tntnet searches for the compiled webapps.

        By default only the working directory and the systems library search
        path are used for the search.

        default: (none)
     */
    CompPathType compPath;

    /** Size (in bytes) of data bundled for a system call

        This setting tunes the size of the input and output buffer for the
        socket. Each connection allocates that many bytes for the buffer.

        default: 16384
     */
    unsigned socketBufferSize;

    /** The timeout (in milliseconds) for reading data from a client

        If keep-alive is activated (default), tntnet starts waiting for requests
        on every new socket after the initial request. To reduce context switches
        between threads the corresponding thread waits for new requests for the
        specified time after the initial request and puts the connection into the
        queue of idle connections afterwards.

        default: 10 milliseconds
     */
    unsigned socketReadTimeout;

    /** The timeout (in milliseconds) for writing data to a client

        Writing to clients is entirely done in one thread. Normally writing
        never blocks, but if that happens, this timeout ensures that that
        does not lock the whole server.

        default: 10000 milliseconds
     */
    unsigned socketWriteTimeout;

    /** The timeout (in milliseconds) for keeping a TCP connection alive

        Per default, keep-alive connections are used, which means TCP connections
        between server and client stay for multiple requests. This option controls
        how long TCP connections are kept alive.

        default: 15000
     */
    unsigned keepAliveTimeout;

    /** Maximal amount of requests per TCP connection in keep alive

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

        If listening on a local interface fails, tntnet will
        retry for the specified amount of times.

        default: 5
     */
    unsigned listenRetry;

    // TODO: Where do the 10% come from? Was this an old behaviour replaced
    // by minCompressSize or is this an additional (unconfigurable) check?
    /** Whether to enable gzip compression for data sent to the client

        Unless this is set to false, tntnet compresses all data using gzip if
        @li the data can be compressed by more than 10% and
        @li the client sends the flag "Accept-Encoding" in the http request header

        default: true
     */
    bool enableCompression;

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

    /** The default mime-type for the http header

        Sets the content type header of the reply. The content type may be changed in
        the application using `reply.setContentType("something")`.

        default: "text/html; charset=UTF-8"
     */
    std::string defaultContentType;

    /** Logfile for server requests

        The format of the logging entries is compatible with logging tools for http servers.

        If this option is unset (empty), no logfile is written.

        default: (empty)
     */
    std::string accessLog;

    /** Logfile for errors

        This option redirects stderr to the specified file.
        It is crucial to set this if you run tntnet as daemon.

        If this option is unset (empty), no logfile is written.

        default: (empty)
     */
    std::string errorLog;

    /** The timer interval, when session timeout are monitored.

        A timer thread periodically checks whether a session times out.
        This setting specifies how often this is checked.

        default: 10 (seconds)
     */
    unsigned timerSleep;

    /** Contains the whole configuration as a cxxtools::SerializationInfo

        cxxtools::SerializatioInfo is a generic structure which can be used
        to read custom information from the configuration file.
        This member is automatically set when deserializing a file to a
        TntConfig object and should not be used otherwise.

        default: (empty object)
     */
    cxxtools::SerializationInfo config;

    /** Sets environment variables.

        A std::map of environment variables to set when Tntnet starts.

        default: (empty map)
     */
    EnvironmentType environment;

    /** The directory under which the component static\@tntnet searches for files

        If this option is unset (empty), static\@tntnet searches
        for files in the working directory.

        default: (empty)
     */
    std::string documentRoot;

    /** Sets the Server header in http reply.

        The server header in the http reply is normally set to the used server
        software. For Tntnet, the default is "Tntnet" plus the version number
        but you can override that. The Server header will get omitted if this
        option is unset (empty).

        default: Tntnet/'version-number'
     */
    std::string server;

    /** Files from which to read additional options

        Through this option you can specify additional files from which
        more configuration options should be read.

        default: (none)
     */
    std::vector<std::string> includes;

    /// Create a %TntConfig object with default configuration
    TntConfig();

    /// Check whether a setting with the specified key is set in the configuration
    bool hasValue(const std::string& key) const
      { return config.findMember(key) != 0; }

    /** Returns the instance of %TntConfig used by the tntnet server at startup

        When starting tntnet (the executable), configuration is read from a xml or
        json file. Applications get access to the configuration through this object.
     */
    static TntConfig& it();
  };

  /// Deserialization operator for TntConfig::Mapping
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Mapping& mapping);
  /// Deserialization operator for TntConfig::Listener
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::Listener& listener);
  /// Deserialization operator for TntConfig::SslListener
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig::SslListener& ssllistener);
  /// Deserialization operator for TntConfig
  void operator>>= (const cxxtools::SerializationInfo& si, TntConfig& config);
}

#endif // TNT_TNTCONFIG_H

