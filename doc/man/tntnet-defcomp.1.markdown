tntnet-defcomp 1 "2013-05-27" Tntnet "Tntnet users guide"
=========================================================

NAME
----

tntnet - default components for tntnet(8)

DESCRIPTION
-----------

By default tntnet can just return a http error 404 - not found. To do something
meaningful tntnet loads shared libraries with components. By defining suitable
mappings in tntnet.xml(7) tntnet loads those components, which does the actual
work.

Since in the web world there are some really basic tasks, which every web server
should be able to do, tntnet brings some standard components. The shared
library, which does that is called `tntnet` and hence every standard component
has a component identifier of *componentname*`@tntnet`. They are documented
here.

### static

The most commonly used standard component is `static@tntnet`. It gives tntnet
the ability to deliver files from the file system. For this it uses the
`pathinfo` setting from the mapping in tntnet.xml(7) as a file name and delivers
the files. The extension of the files is used to look up the content type. The
same mime database is used as in the next component `mime@tntnet`.

*Example*

    <mapping>
      <url>(.*)</url>
      <target>static@tntnet</target>
      <pathinfo>/var/www/htdocs/$1</pathinfo>
    </mapping>

This setting configures tntnet as simple web server for static pages.

### mime

The component `mime@tntnet` sets just the content type header. The value is
fetched from the argument "contenttype" if set. Otherwise the path info is used
to detect the correct content type from the file extension using the configured
mime db.

### empty

The component `empty@tntnet` generates just an empty result. By default a http
return code OK (200) is set but can be changed with the argument _httpcode_.
Other arguments are interpreted as additional http headers, so that e.g. the
content type header can be set.

*Example*

    <mapping>
      <url>\.js$</url>
      <target>empty@tntnet</target>
      <args>
        <ContentType>application/javascript</ContentType>
      </args>
    </mapping>

This tells tntnet to reply all requests with a url ending _.js_ with an empty
javascript file.

### unzip

The component `unzip@tntnet` reads static data from a zip file. The file name is
read from the argument "file" and the actual file from the path info.

If the argument "contenttype" is set, the content type http header is set from
that value. Otherwise the path info is used to look up the content type from the
file extension using the configured mime db.

*Example*

    <mapping>
      <url>/thefile/(.*)</url>
      <target>unzip@tntnet</target>
      <pathinfo>$1</pathinfo>
      <args>
        <file>/var/www/thefile.zip</file>
      </args>
    </mapping>

Reads file from the specified zip file when the url starts with `/thefile/`. The
actual file in the zip file is read from the rest of the url. The content type
header is set according the the file extenxion and the file is sent to the
client.

### redirect

The component `redirect@tntnet` just returns with a redirect to another page.
The location for the redirect is specified in the `<pathinfo>` setting.

By default a temporary redirect code (301) is sent, but can be configured by
adding a configuration argument "type". The value of the type can be
"temporarily", "permanently" or a number, which is used.

*Example*

    <mapping>
      <url>^/$</url>
      <target>redirect@tntnet</target>
      <redirect>/login.html</redirect>
    </mapping>

This setting redirects the client to the index.html file when the root directory
is requested.

*Example for a permanent redirect*

    <mapping>
      <url>^/$</url>
      <target>redirect@tntnet</target>
      <redirect>/index.html</redirect>
      <args>
        <type>permanently</type>
      </args>
    </mapping>

This setting redirects the client to the index.html file when the root directory
is requested.

### error

The error component returns a specific error code to the client when called. The
code is specified using the argument `<code>` and the message using the argument
`<message>`. The message may be omitted in which case a default error code
specific text is used.

*Example*

    <mapping>
      <method>^POST$</method>
      <target>error@tntnet</target>
      <args>
        <code>405</code>
      </args>
    </mapping>

This setting prevents tntnet to accept POST message.

AUTHOR
------
This manual page was written by Tommi Mäkitalo <tommi@tntnet.org>.

SEE ALSO
--------

tntnet-project(1), ecpp(7), ecppc(1), tntnet.xml(7),
