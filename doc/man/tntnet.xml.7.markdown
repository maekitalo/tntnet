tntnet.xml 7 "2006-07-23" Tntnet "Tntnet users guide"
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

`<accessLog>`*filename*`</accessLog>`

  Writes a log entry for each request in a common format. This format is
  compatible with most log file analyze systems for http servers.

  The log file has the fields: `peer ip` - `username` [`time`] "`http method`
  `query string` HTTP/`major version`.`minor version`" `http return code`
  `content size` "`referer`" "`user agent`"

  The `username`, `referer` and `user agent` may be '-' when the value is not
  available. Also the `content-size` can be empty in some cases.

  *Example*

    <accessLog>/var/log/tntnet/access.log</accessLog

`<bufferSize>`*bytes*`</bufferSize>`

  Specifies the number of bytes sent in a single system call. This does not
  limit anything in application level. It does not affect e.g. savepoints or
  exception handling. Component output is collected completely and then passed
  in chunks of `bufferSize` bytes to the operating system.

  The default value is 16384.

`<compPath>` [ `<entry>`*path1*`</entry>` ] `</compPath>`

  `comppath` specifies, where tntnet should search for webapplications. Tntnet
  searches first in the current directory and then in each directory, you
  specify here, until a library is found. You can repeat the directive as many
  times as desired to add more entries. If it is not found, the next
  `mappings` entry is tried.

  *Example*

    <comppath>
      <entry>/usr/local/lib/tntnet</entry>
      <entry>/usr/local/share/tntnet</entry>
    </comppath>

`<chroot>`*directory*`</chroot>`

  Does a chroot(2) system call on startup, which locks the process into the
  directory at system level.

  *Example*

    <chroot>/var/tntnet</chroot>

`<daemon>`*0|1*`</daemon>`

  If this flag is set to 1, Tntnet forks at startup and terminates the
  parent process on successful initialization.

`<dir>`*directory*`</dir>`

  Changes the current working directory of the process on startup.

  *Example*

    <dir>/var/tntnet</dir>

`<defaultContentType>`*contentType*`</defaultContentType>`

  Sets the content type header of the reply. The content type may be changed in
  the application using `reply.setContentType("something")`.

  By default "text/html; charset=UTF-8" is set.

  *Example*

    <defaultContentType>text/html; charset=ISO-8858-1</defaultContentType>

`<enableCompression>`*yes|no*`</enableCompression>`

  Specifies, if Tntnet should use gzip compression at http level. By default
  Tntnet use compression. A http client like a web browser can send a header
  "Accept-Encoding", to tell Tntnet, that it would accept compressed data.
  Tntnet then can decide, if it use compression. When the body is complete,
  Tntnet tries to compress the body. If the data can be compressed by more than
  10%, Tntnet sends this compressed data. With this flag, this feature can be
  turned off.

  Compression slows down processing but reduces the network load. Normally the
  size of html pages can be compressed by about 70%, while Tntnet slows down by
  up to 30%.

  *Example*

    <enableCompression>no</enableCompression>

`<environment>` `<name1>`*value1*`</name1>` `<name2>`*value2*`</name2>` `</environment>`

  Sets environment variables.

`<errorLog>`*filename*`</errorLog>`

  Redirects stderr to the specified file when tntnet runs as a daemon. If
  ErrorLog is not set stderr is redirected to /dev/null.

  *Example*

    <errorLog>/var/log/tntnet/error.log</errorLog>

`<group>`*unix-group-id*`</group>`

  Changes the group under which tntnet runs.

  The user is changes using the system call setgid(2), which is only allowed,
  when tntnet starts as root user.

  *Example*

    <group>tntnet-group</group>

`<keepAliveTimeout>`*milliseconds*`</keepAliveTimeout>`

  Sets the timeout for keep-alive requests.

  Tntnet tries to do keep-alive-requests wherever possible. This has the effect,
  that tntnet can receive multiple requests within a single tcp connection. The
  connection times out after KeepAliveTimeout milliseconds. The timeout defaults
  to 15000ms.

  *Example*

    <keepAliveTimeout>300000</keepAliveTimeout>

`<keepAliveMax>`*number*`</keepAliveMax>`

  Sets the maximum number of request per tcp connection. This defaults to 100.

  *Example*

    <keepAliveTimeout>10</keepAliveTimeout>

`<listeners>`*listener definition*`</listeners>`

  Specifies, on which local interfaces tntnet waits for connections. There can
  be more than one Listen directives, in which case tntnet waits on every
  address.

  See separate section *Listeners*

`<logging>`*logger definition*`</logging>`

  Configures logging. See separate section *logging*

`<listenRetry>`*number*`</listenRetry>`

  On startup Tntnet calls listen on the specified port. When the systemcall
  returns with an error, Tntnet tries again and fails after the specified number
  of attempts.

  The default number is 5.

  *Example*

    <listenRetry>10</listenRetry>

`<listenBacklog>`*number*`</listenBacklog>`

  The system call listen(3p) needs a parameter backlog, which specifies, how
  many pending connections the operating system should queue before it starts to
  ignore new request. The value is configurable here.

  The default value is 16.

  *Example*

    <ListenBacklog>64</ListenBacklog>

`<mappings>`*urlmappings*`</mappings>`

  This is the most important setting for tntnet. It specifies, which components
  schould be called on which urls.

  For details see the section *URL MAPPING*.

`<maxUrlMapCache>`*number*`</maxUrlMapCache>`

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

`<maxRequestSize>`*number*`</maxRequestSize>`

  This directive limits the size of the request. After *number* Bytes the
  connection is just closed. This prevents denial of service attacks through
  long requests. Every request is read into memory, so it must fit into it.
  Bear in mind, that if you use file upload fields a request might be larger
  than just a few bytes.

  The value defaults to 0, which means, that there is no limit at all.

  *Example*

    <maxRequestSize>65536</maxRequestSize>

`<maxRequestTime>`*seconds*`</maxRequestTime>`

  In daemon mode tntnet has a watchdog, which restarts tntnet when the maximum
  request time is exceeded. This happens, when a request is in a endless loop or
  otherwise hangs. Restarting tntnet looses all active sessions and the
  currently running requests. Therefore the timeout should be well long enough
  for the longes request.

  The default value is 600 seconds, which is normally much longer than a http
  request should run. If the Timeout is set to 0, the watchdog is deactivated.

  *Example*

    <maxRequestTime>1200</maxRequestTime>

`<minThreads>`*number*`</minThreads>`

  Tntnet uses a dynamic pool of worker threads, which wait for incoming
  requests. MinThreads specifies, how many worker threads there have to be. This
  defaults to 5.

  *Example*

    <minThreads>10</minThreads>

`<minCompressSize>`*number*`</minCompressSize>`

  Http compression for replies smaller than this are not compressed at all.

  The default value for this is 1024.

  *Example*

    <minCompressSize>256</minCompressSize>

`<mimeDb>`*filename*`</mimeDb>`

  Specify filename for mime db. The default is /etc/mime.types.

  The format of the file is just like this /etc/mime.types. A mime type is
  followed after white space by a list of file extensions delimited by white
  space.

`<maxThreads>`*number*`</maxThreads>`

  Tntnet uses a dynamic pool of worker threads, which wait for incoming
  requests. `maxThreads` limits the number of threads.


  The default is 100.

  *Example*

    <maxThreads>200</maxThreads>

`<pidfile>`*filename*`</pidfile>`

  When run in daemon mode, tntnet writes the process id of the monitor process
  to filename. When the monitor process is deactivated, the pid of the
  worker process is written. This ensures, that sending a sigkill to the the
  stored process id stops tntnet.

  *Example*

    <pidfile>/var/run/tntnet.pid</pidfile>

`<queueSize>`*number*`</queueSize>`

  Tntnet has a request queue, where new requests wait for service. This sets a
  maximum size of this queue, after wich new requests are not accepted.

  The default value is 1000.

  *Example*

    <queueSize>50</queueSize>

`<reuseAddress>`*0|1*`</reuseAddress>`

  The flag specifies whether the socket option SO\_REUSEADDR should be set.
  When the entry is omitted the flag is set.

`<server>`*name*`</server>`

  Set the server response header. Tntnet sets the http header "Server:" to
  "Tntnet/version" by default. Whith this setting the header can be changed.

  *Example*

    <server>Myserver version 1.2</server>

`<sessionTimeout>`*seconds*`</sessionTimeout>`

  This sets the number of seconds without requests after which a sesssion is
  timed out.

  The default value is 300 seconds.

  *Example*

    <sessionTimeout>600</sessionTimeout>

`<socketReadTimeout>`*milliseconds*`</socketReadTimeout>`

  A worker thread waits for some milliseconds on incoming data. If there is no
  data, the job is put into a queue and another thread waits with poll(2) on
  incoming data on multiple sockets. The workerthreads are freed and they can
  respond to other requests quickly. The default value is 10 milliseconds, which
  is good for normal operation. A value of 0 results in non blocking read. If
  timeout is reached, this does not mean, that the socket is closed. A small
  timeout reduces contextswitches on slow connections.

  *Example*

    <socketReadTimeout>0</socketReadTimeout>

`<socketWriteTimeout>`*milliseconds*`</socketWriteTimeout>`

  This defines the time, how long the workerthreads wait on write.  If the
  timeout is exceeded, the socket is closed and the browser might not get all
  data.  The default value is 10000 milliseconds.

  *Example*

    <socketWriteTimeout>20000</socketWriteTimeout>

`<threadStartDelay>`*ms*`</threadStartDelay>`

  When additional worker threads are needed tntnet waits the number of
  milliseconds before it starts additional threads to prevent high load when
  starting many threads at once.

  The default value is 10ms.

  *Example*

    <threadStartDelay>1000</threadStartDelay>

`<user>`*username*`</user>`

  Changes the user under which tntnet answers requests.

  The user is changes using the system call setuid(2), which is only allowed,
  when tntnet starts as root user.


  *Example*

    <user>www-data</user>

`<virtualhosts> { <virtualhost> <hostname>`*hostname-regex*`</hostname>`*mappings*` </virtualhost> }</virtualhosts>`

  Defines mappings for virtual hosts. These mappings are valid only when the
  host header matches the *hostname-regex*. See section *URL MAPPING* for
  details about how to define actual mappings

  A *vhost* entry in the mappings should be empty since it is already specified
  for the whole group.

  The mappings defined here are always matched before the mappings on the top
  level of the configuration.

  *Example*

    <virtualhosts>
      <virtualhost>
        <hostname>www\.tntnet\.org</hostname>
        <mappings>
          <mapping>
            <url>^/$</url>
            <target>static@tntent</target>
            <pathinfo>htdocs/index.html</pathinfo>
          </mapping>
          <mapping>
            <url>^/(.*)$</url>
            <target>static@tntent</target>
            <pathinfo>htdocs/$1</pathinfo>
          </mapping>
        </mappings>
      </virtualhost>
    </virtualhosts>

URL MAPPING
-----------
Tntnet is a web server, which receives http requests from a http client and
answers them. A http request has a url and other attributes, which are used to
decide, how the answer should look like. This is done my mapping urls to components.

A component is something, which generates a http reply. They are normally
generated with the ecpp compiler ecppc(1). The ecppc compiler generated C++
classes with component names. The classes are compiled and linked into a shared
library. Both the component name and the shared library name is needed to
identify a component.

The component identifier is a string built from the component name, the @
character and the shared library name. A example is `myclass@myapplication`.
This tells tntnet: load shared library `myapplication` and call the component
with the name `myclass` in that library, which creates the reply to the request.

To tell tntnet, which component to call, url mappings must be configured.

Configuration is done in the xml section `<mappings>`. Multiple mappings can be
configured there. A mapping has a condition and a target. Tntnet looks in the
list of mappings for the first mapping, where the condition is met and uses that
to call the component. The component may return either a reply - then the
request is done or a special value `DECLINED`, which tells tntnet to continue in
the list and look for the next mapping, where the condition is met.

The component, which returns `DECLINED` may already have generated part of the
request. This is preserved for the next mapping. A common use case is to write a
special component, which just checks the user name and password. If the user
name and password is valid, `DECLINED` is returned and tntnet calls the next
mapping where the condition is met.

Also when the condition is met, but the component could not be loaded, tntnet
continues with the next mapping.

When the end of the list is reached and no mapping returned a http reply code,
tntnet replies with http not found (404) error.

So how these mapping are specified then?

The mapping contains 3 kind of nodes:

`conditions`
  Multiple conditions can be specified. All conditions must be met when the
  mapping is to be used.

  The most important is `<url>`, which contains a extended regular expression
  (see regex(7) for details). This expression is checked against the url of the
  request. If the url tag is omitted, the mapping is used for every url.

  The condition `<vhost>` specifies the virtual host, for which this mapping is
  valid. When this is specified, the mapping is only valid for requests, where
  the virtual host matches the setting. The value is also a extended regular
  expression. Note, that a dot matches any character in regular expressions,
  which may be irritating here. If you want to specify a mapping for the all
  hosts of the domain `tntnet.org`, you have to set
  `<vhost>tntnet\.org$</vhost>`. Also the dollar sign at the end is important,
  since it matches the end of the string. Otherwise the mapping would be also
  valid for a virtual host like `tntnet.org.foo.com`, which may not be what you
  meant.

  The condition `method` specifies the http method for which the mapping should
  be considered. Again a extended regular expression is used.

  The condition `ssl` is a boolean value. The value should be 0 or 1. The
  setting checks, whether this mapping should be used depending on ssl.  If the
  value is 1, the condition is met, when the request is sent via ssl. If the
  value is 0, the condition is met, when the request is sent without ssl.

`target`
  The mapping node contains a node `<target>`, which contains the component name,
  which is to be called when the conditions are met.

  The target may contain back references to the regular expression in the
  `<url>` condition. Parts of the regular expression may be in brackets. In the
  target $1 is replaced with the first bracketed expression, $2 with the second
  and so on.

  This target node is mandatory.

  Optionally there may be another node `<httpreturn>`, which specifies the
  default http return code of components. The component may return with the
  special constant DEFAULT, which instructs tntnet to return the default http
  return code for this mapping. The ecpp compiler generates components, which
  return exactly that value and the default value is HTTP\_OK. The value of the
  node may be a numeric http return code or the word DECLINED, which instructs
  tntnet to continue with the next mapping.

`parameters`
  When the condition is met, additional parameters may be passed to the called
  component. There are 2 nodes for this.

  The node `<pathinfo>` can be requested in the component using
  `request.getPathInfo()`. If the node is not set, the url is set as path info.

  The node `<args>` contains additional parameters, which can be passed to the
  component. The node can have any number of nodes with values. The tags are
  used as a parameter name and the content as the value. The method
  `request.getArg(`*name*`)` returns the value of the specified *name*. When the
  node is not set, the method returns a empty string. Optionally a diffrent
  default value can be passed to the method as an additional parameter like
  `request.getArg(`*name*`, `*defaultValue*`)`.

  For compatibility reasons with older tntnet `request.getArg` accepts a numeric
  argument. Previously the arguments did not have names but were accessed by
  index. To emulate this, `request.getArg` with a numeric argument translates
  the number into the name "`arg`*number*". So accessing
  `request.getArg(`*2*`)` returns the value of the argument with the name
  `arg2`. Accessing a numeric argument equal or greater than the number of
  arguments (the first is number 0) used to be not allowed. Now a empty string
  is returned.

*Example*

    <mappings>
      <!-- map / to index@myapp -->
      <mapping>
        <target>index@myapp</target>
        <url>^/$</url>
        <pathinfo>index.html</pathinfo>
      </mapping>
      <!-- map /comp.* or /comp to comp@myapp -->
      <mapping>
        <target>action@myapp</target>
        <url></url>               <!-- any url -->
        <method>POST</method>     <!-- but only on method POST -->
        <vhost>localhost</vhost>  <!-- and host header must be localhost -->
        <ssl>1</ssl>              <!-- and ssl is enabled -->
      </mapping>
      <mapping>
        <target>$1@myapp</target>
        <url>^/([^.]+)(\.(.+))?</url>
        <args>
          <extension>$2</extension>
        </args>
      </mapping>
    </mappings>

LISTENERS
---------
The section `<listeners>` specifies the ip addresses and ports, where tntnet
waits for incoming requests. Multiple listeners may be defined, when tntnet
should listen on multiple ip addresses or ports.

Each listener is defined in a node `<listener>`. A listener must have a subnode
`<port>`. The node `<ip>` may contain a ip address or hostname or may be left
empty or even omitted. If the node is empty, any interface is used. The `<port>`
must contain the numeric port number.

The ip address may be a IPv4 or IPv6 address.

*Example*

    <listeners>
      <listener>
        <port>80</port>
      </listener>
    </listeners>

SSL
---
Optionally a tag `<certificate>` may be added. This enables ssl on the interface
and specifies the ssl host certificate for the interface.

The ssl key can be stored in a separate file. A tag `<key>` specifies, where to
find it. It is really encouraged to do so and to remove group and other
readability from the key file.

*Example*

    <listeners>
      <listener>
        <port>443</port>
        <!-- a certificate enables ssl -->
        <certificate>tntnet.pem</certificate>
        <key>tntnet.key</key>
      </listener>
    </listeners>

Client certificates are supported. The tags `<sslCa>` and `<sslVerifyLevel>` can
be added to specify the type of check. If `<sslVerifyLevel>` to `1`, a client
certificate is requested but tntnet accepts, if the client sends no certificate.
The application can check the certificate using `request.getSslCertificate()`.
It returns a certificate object, which may be empty.

If `<sslVerifyLevel>` is `2`, a client certificate is mandatory. A connection
without a certificate is not accepted.

In any case the client certificate is verified against the CA specified by
`<sslCa>`.

*Example*

    <listeners>
      <listener>
        <port>443</port>
        <certificate>tntnet.pem</certificate>
        <key>tntnet.key</key>
        <sslVerifyLevel>2</sslVerifyLevel>
        <sslCa>myCa.pem</sslCa>
      </listener>
    </listeners>

With this setting clients has to send a certificate signed by the CA `myCa.pem`.

LISTENER SETTINGS
-----------------
We summarize the settings for the listeners here.

`<ip>`*ip*`</ip>`

    Optional setting for the ip address where tntnet should listen on.

`<port>`*port*`</port>`

    Mandatory setting on which tntnet should listen on.

`<certificate>`*certificate*`</certificate>`

    A certificate file, which contains the ssl server certificate for ssl.

`<key>`*key*`</key>`

    The private key for the server certificate. If the key is not specified,
    tntnet expects the key in the certificate file.

`<sslVerifyLevel>`*0|1|2*`</sslVerifyLevel>`

    Enables client certificates. When set to 0, no client certificate is needed.
    When set to 1 a client certificate is requested but optional. It is just
    passed to the application when available.
    When set to 2 a client certificate is requested and needed to connect.

`<sslCa>`*CA-file*`</sslCa>`

    The certificate authority the client certificate is checked against. Only
    client certificates, which can be verified by this authority are accepted.
    This settins is mandatory when `sslVerifyLevel` is set to 1 or 2.

LOGGING
-------
Logging is configured in the `<logging>` section.

Every log output has a category and a level. Categories are hierarchical.

The configuration tells which category is output at which level and where the
output is written to.

The main log levels are in decreasing severity _FATAL_, _ERROR_, _WARN_, _INFO_,
_DEBUG_, _FINER_, _FINEST_. The severity _FINE_ is the same as _DEBUG_. A
special level is _TRACE_.

The settings are:

`<rootlogger>`*level*`</rootlogger>`

    Sets the main level used, when nothing else is specified. The default is
    _FATAL_.

`<loggers>`*loggers*`</loggers>`

    Specifies the log levels for a category. See separate section about
    *loggers* for details.

`<file>`*filename*`</file>`

    Log output is written to file. Default is *stderr*.

`<maxfilesize>`*size*`</maxfilesize>`

    Specifies maximum _size_ of file. The size is a number in bytes or it can be
    prepended by a unit of 'k', 'm' or 'g', which specified kilo, mega or
    gigabytes. The unit is not case sensitive.

    After reaching the size, a log message is written into the current file and
    renamed by appending a '.0' to it. A new file is then created.

`<maxbackupindex>`*number*`</maxbackupindex>`

    If the *maxfilesize* was reached and there is already a file with a *.0*
    extension it is renamed to .1. This happens up to the specified number. So
    if *maxfilesize* is e.g. 4 the log files are rolled up to .4, so that 5
    files are kept.

`<logport>`*portnumber*`</logport>`

    When a *portnumber* is specified and no *file* is specified (*file* has
    higher priority), log messages are sent to the specified udp port.

`<loghost>`*host*`</loghost>`

    When *logport* and *loghost are set, the messages are sent to the host.

`<broadcast>`*boolean*`</broadcast>`

    When the host address is a network address, the *broadcast* flag must be
    set. Then the messages are sent as broadcast udp messages to the network.

    A boolean value is false when it is empty or starts with '0', 'f', 'F', 'n'
    or 'N'.

`<stdout>`*boolean*`</stdout>`

    When no file or loghost is set output and this flag is set, the output is
    written to *stdout*. The default is *stderr*.

    A boolean value is false when it is empty or starts with '0', 'f', 'F', 'n'
    or 'N'.

### loggers

In the section `<loggers>` a list of `<logger>` is specified. A logger has this
settings:

`<category>`*category*`</category>`

    Specifies the category

`<level>`*level*`</level>`

    Specifies the level for this category.
    
    The level can be one of the log levels _FATAL_, _ERROR_, _WARN_, _INFO_,
    _DEBUG_, _FINE_, _FINER_ or _FINEST_. The all messages with a level of this
    or higher are output. So e.g. setting _DEBUG_ outputs all messages from
    _FATAL_ to _DEBUG_.

    It is enough to specify the first character (except _FINE_, _FINER_ and
    _FINEST_).

    Setting _TRACE_ (or _T_) is like _DEBUG_ plus all _TRACE_ messages. _TRACE_
    is special since it outputs a message when a the code is reached and a exit
    message when the scope of the code is exited. Typically the _TRACE_ messages
    are put at the start of functions so that the log shows when a function is
    called and when it is exited.

    Multiple log levels can be separated by '|'. In that case only those levels
    are output. E.g. _WARN|DEBUG_ prints just warning and debug messages. When
    just one level should be printed, it can be prepended by '|', so e.g.
    _|ERROR_ prints just error messages.

    Prepending a 'T' to the log level adds always traces. E.g. *TINFO* prints
    info messages and above and traces. _TFINEST_ prints just everything.

Note that all components has a automatic log category of _component.*name*_ set.

### logger examples

    <loggers>
        <logger>
            <category>tntnet</category>
            <level>DEBUG</level>
        </logger>
    </loggers>

Prints much debug information when tntnet runs.

    <loggers>
        <logger>
            <category>tntnet.dispatcher</category>
            <level>DEBUG</level>
        </logger>
        <logger>
            <category>component</category>
            <level>TRACE</level>
        </logger>
    </loggers>

Prints debug messages about the processing of url mappings and traces about
called (ecpp-)components.

Other useful categories are *tntnet.messageheader*, which outputs debug messages
for each header sent from the client or *tntnet.httpreply*, where we can see the
reply headers.

AUTHOR
------
This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------
tntnet (1)
