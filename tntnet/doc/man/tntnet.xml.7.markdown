tntnet.xml 8 "2006-07-23" Tntnet "Tntnet users guide"
=====================================================

NAME
----
tntnet.xml - configuration file for tntnet(8)

DESCRIPTION
-----------
Tntnet is configured using a xml file. The name of the file is *tntnet.xml*. The
root node of *tntnet.xml* should be `tntnet` while it is not checked. Most of
the settings are just single values. They are described here in alphabetical
order.

SETTINGS
--------
This section describes the variables, used by Tntnet (8).

`<AccessLog>*filename*</AccessLog>`
  Writes a log entry for each request in a common format. This format is
  compatible with most log file analyze systems for http servers.

  The log file has the fields:
    `peer-ip` - `username` [`time`] "`http-method` `query-string` HTTP/`major-version`.`minor-version`" `http-return-code` `content-size` "`referer`" "`user-agent`"

  The `username`, `referer` and `user-agent` may be '-' when the value is not
  available. Also the `content-size` can be empty in some cases.

  *Example*

    <AccessLog>/var/log/tntnet/access.log</AccessLog

`<BufferSize>*bytes*</BufferSize>`

  Specifies the number of bytes sent in a single system-call. This does not
  limit anything in application-level. It does not affect e.g. savepoints or
  exception-handling. Component-output is collected completely and then passed
  in chunks of BufferSize bytes to the operating system.

  The default value is 16384.

`<comppath><entry>*path1*</entry></comppath>`

  `comppath` specifies, where tntnet should search for webapplications. Tntnet
  searches first in the current directory and then in each directory, you
  specify here, until a library is found. You can repeat the directive as many
  times as desired to add more entries. If it is not found, the next
  MapUrl-entry is tried.

  *Example*

    <comppath>
      <entry>/usr/local/lib/tntnet</entry>
      <entry>/usr/local/share/tntnet</entry>
    </comppath>

`<chroot>*directory*</chroot>`

  Does a chroot(2)-system call on startup, which locks the process into the directory at system-level.

  *Example*

    <chroot>/var/tntnet</chroot>

`<daemon>*0|1*</daemon>`

  If this flag is set to 1, Tntnet forks at startup and terminates the
  parent-process on successful initialization.

`<dir>*directory*</dir>`

  Changes the current working directory of the process on startup.

  *Example*

    <dir>/var/tntnet</dir>

`<enableCompression>*yes|no*</enableCompression>`

  Specifies, if Tntnet should use gzip-compression at http-level. By default
  Tntnet use compression. A http-client like a web browser can send a header
  "Accept-Encoding", to tell Tntnet, that it would accept compressed data.
  Tntnet then can decide, if it use compression. When the body is complete,
  Tntnet tries to compress the body. If the data can be compressed by more than
  10%, Tntnet sends this compressed data. With this flag, this feature can be
  turned off.

  Compression slows down processing but reduces the network-load. Normally the
  size of html-pages can be compressed by about 70%, while Tntnet slows down by
  up to 30%.

  *Example*

    <enableCompression>no</enableCompression>

`<errorLog>*filename*</errorLog>`

  Redirects stderr to the specified file when tntnet runs as a daemon. If
  ErrorLog is not set stderr is redirected to /dev/null.

  *Example*

    <errorLog>/var/log/tntnet/error.log</errorLog>

`<group>*unix-group-id*</group>`

  Changes the group under which tntnet runs.

  The user is changes using the system call setgid(2), which is only allowed,
  when tntnet starts as root user.

  *Example*

    <group>tntnet-group</group>

`<keepAliveTimeout>*milliseconds*</keepAliveTimeout>`

  Sets the timeout for keep-alive requests.

  Tntnet tries to do keep-alive-requests wherever possible. This has the effect,
  that tntnet can receive multiple requests within a single tcp-connection. The
  connection times out after KeepAliveTimeout milliseconds. The timeout defaults
  to 15000ms.

  *Example*

    <keepAliveTimeout>300000</keepAliveTimeout>

`<keepAliveMax>*number*</keepAliveMax>`

  Sets the maximum number of request per tcp-connection. This defaults to 100.

  *Example*

    <keepAliveTimeout>10</keepAliveTimeout>

`<listeners>*listener definition*</listeners>`

  Specifies, on which local interfaces tntnet waits for connections. There can
  be more than one Listen-directives, in which case tntnet waits on every
  address.

  See separate section *Listeners*

`<logging>*listener definition*</logging>`

  Configures logging. See separate section *logging*

`<listenRetry>*number*</listenRetry>`

  On startup Tntnet calls listen on the specified port. When the systemcall
  returns with an error, Tntnet tries again and fails after the specified number
  of attempts.

  The default number is 5.

  *Example*

    <listenRetry>10</listenRetry>

`<listenBacklog>*number*</listenBacklog>`

  The system-call listen(3p) needs a parameter backlog, which specifies, how
  many pending connections the operating-system should queue before it starts to
  ignore new request. The value is configurable here.

  The default value is 16.

  *Example*

    <ListenBacklog>64</ListenBacklog>

`<mapUrl>*urlmappings*</mapUrl>`

  This is the most important setting for tntnet. It specifies, which components
  schould be called on which urls.

  For details see the section *Urlmapping*.

`<maxUrlMapCache>*number*</maxUrlMapCache>`

  Mapping urls to components is done using regular expressions. Executing these
  expressions is quite expensive while the number of different urls is quite
  limited in typical web applications. Hence tntnet caches the results.

  The caching algorithm is very simple. Tntnet just collects the results in a
  map. When the maximum size of the list is reached, it is cleared. This makes
  management of the cache very cheap.

  This setting sets the maximum number of entries in the map.

  If you see frequently a warning message, that the cache is cleared, you may
  consider increasing the size.

  The default value is 8192.

  *Example*

    <maxUrlMapCache>32768</maxUrlMapCache>

`<maxRequestSize>*number*</maxRequestSize>`

  This directive limits the size of the request. After *number* Bytes the
  connection is just closed. This prevents denial-of-service-attacks through
  long requests. Every request is read into memory, so it must fit into it.
  Bear in mind, that if you use file-upload-fields a request might be larger
  than just a few bytes.

  The value defaults to 0, which means, that there is no limit at all.

  *Example*

    <maxRequestSize>65536</maxRequestSize>

`<maxRequestTime>*seconds*</maxRequestTime>`

  In daemon mode tntnet has a watchdog, which restarts tntnet when the maximum
  request time is exceeded. This happens, when a request is in a endless loop or
  otherwise hangs. Restarting tntnet looses all active sessions and the
  currently running requests. Therefore the timeout should be well long enough
  for the longes request.

  The default value is 600 seconds, which is normally much longer than a http
  request should run. If the Timeout is set to 0, the watchdog is deactivated.

  *Example*

    <maxRequestTime>1200</maxRequestTime>

`<minThreads>*number*</minThreads>`

  Tntnet uses a dynamic pool of worker-threads, which wait for incoming
  requests. MinThreads specifies, how many worker threads there have to be. This
  defaults to 5.

  *Example*

    <minThreads>10</minThreads>

`<minCompressSize>*number*</minCompressSize>`

  Http-compression for replies smaller than this are not compressed at all.

  The default value for this is 1024.

  *Example*

    <minCompressSize>256</minCompressSize>

`<maxThreads>*number*</maxThreads>`

  Tntnet uses a dynamic pool of worker-threads, which wait for incoming
  requests. `maxThreads` limits the number of threads.


  The default is 100.

  *Example*

    <maxThreads>200</maxThreads>

`<pidFile>*filename*</pidFile>`

  When run in daemon-mode, tntnet writes the process-id of the monitor-process
  to filename. When the monitor-process is deactivated, the pid of the
  worker-process is written. This ensures, that sending a sigkill to the the
  stored process-id stops tntnet.

  *Example*

    <pidFile>/var/run/tntnet.pid</pidFile>

`<queueSize>*number*</queueSize>`

  Tntnet has a request-queue, where new requests wait for service. This sets a
  maximum size of this queue, after wich new requests are not accepted.

  The default value is 1000.

  *Example*

    <queueSize>50</queueSize>

`<sessionTimeout>*seconds*</sessionTimeout>`

  This sets the number of seconds without requests after which a sesssion is
  timed out.

  The default value is 300 seconds.

  *Example*

    <sessionTimeout>600</sessionTimeout>

`<socketReadTimeout>*milliseconds*</socketReadTimeout>`

  A worker-thread waits for some milliseconds on incoming data. If there is no
  data, the job is put into a queue and another thread waits with poll(2) on
  incoming data on multiple sockets. The workerthreads are freed and they can
  respond to other requests quickly. The default value is 10 milliseconds, which
  is good for normal operation. A value of 0 results in non-blocking read. If
  timeout is reached, this does not mean, that the socket is closed. A small
  timeout reduces contextswitches on slow connections.

  *Example*

    <socketReadTimeout>0</socketReadTimeout>

`<socketWriteTimeout>*milliseconds*</socketWriteTimeout>`

  This defines the time, how long the workerthreads wait on write.  If the
  timeout is exceeded, the socket is closed and the browser might not get all
  data.  The default value is 10000 milliseconds.

  *Example*

    <socketWriteTimeout>20000</socketWriteTimeout>

`<threadStartDelay>*ms*</threadStartDelay>`

  When additional worker threads are needed tntnet waits the number of
  milliseconds before it starts additional threads to prevent high load when
  starting many threads at once.

  The default value is 10ms.

  *Example*

    <threadStartDelay>1000</threadStartDelay>

`<user>*username*</user>`

  Changes the user under which tntnet answers requests.

  The user is changes using the system call setuid(2), which is only allowed,
  when tntnet starts as root user.


  *Example*

    <user>www-data</user>

Urlmapping
----------

TODO

Listeners
---------

TODO

AUTHOR
------

This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet (1)
