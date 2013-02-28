Static files with Tntnet
========================

Introduction
------------

Tntnet is a web application server for C++. That means, that it is easy to
create dynamic web pages using C++ embedded into HTML. But this does not mean,
that Tntnet can't send static pages. Static pages are almost always needed in
web applications. Css, Images or Javascript libraries are seldom dynamically
generated.  There are several solutions to send static pages. Since there are
different situations and hence different needs, it is not uncommon to use more
than one solution in your application.  We will discuss here the possible
solutions and the strength and caveats of each of them. 

Using the binary mode of ecppc
------------------------------

The simplest solution is to use the binary mode of ecppc. This is
enabled by passing -b to ecppc. The binary mode disables interpretation of ecpp
tags, when the file is compiled. So if the content of the file happens to
contain e.g. the character sequence `<$`, which normally introduces the start
of a c++ expression, it is still sent as is.

As with dynamic content, the mime type is looked up using the extension of the
file. If ecppc happens to fail here and you know better, you may pass the
correct mime type using the -m switch.
  
The advantage here is, that you can handle static files just like dynamic ones.
The static files are also compiled into the web application, so you do not have
to bother, how tntnet finds the source files.

There are 2 caveats though. If you frequently change the static files, you have
to recompile your application. And the memory overhead may be quite
significant.  I've measured additional on one specific system 8k in binary size
for each component. So if you have a quite small static file of e.g. 0.5k, you
  have additional 8.5k in your binary.

Sending static files using a special component
----------------------------------------------

If you really want to read the data from a external source like the file
system, you are always free to send the content from your own written
component. You may pass a file name to the component using a query parameter or
the path info, which can also be set in the url mapping. Let's create a simple
solution:

    <%pre>
    #include <fstream>
    #include <tnt/mimedb.h>
    </%pre>
    <%cpp>
    std::ifstream in(("./" + request.getPathInfo()).c_str());
    if (!in)
      return DECLINED;

    tnt::MimeDb mimeDb("/etc/mime.types");
    reply.setContentType(mimeDb.getMimetype(request.getPathInfo()));

    reply.out() << in.rdbuf();  // send the content of the ifstream to the page
    </%cpp>

We add a mapping to the tntnet.xml:

    <mapping>
      <url>.</url>
      <target>static@static</target>
    </mapping>

This works fine. Notice, that the component sends a DECLINED, if the file is
not found. This tells Tntnet to look for the next url mapping. If tntnet do not
find any mappings any more, it sends HTTP_NOT_FOUND to the browser. That's what
we want to see, if there is no file found.

Instead of getting the path from the path info variable, you may also read it
from a query parameter. But you have to be really careful. The path name should
not break out of the directory you have intended to read the data from. Think
about a path name like "../../../etc/passwd". You don't really want to publish
that file to the web!

Tntnet checks automatically the request path for these double dot attacks, so
the solution above works fine. But it can't check it from query parameters. You
have to do that on your own.

Let's look at the advantages using this solution.

It is totally under your control, what to send when. You are not limited to
read data from a file but you can also read the data e.g. from a database.

The disadvantage is, that it is quite difficult to do correctly.

Using static@tntnet
-------------------

Since it is so common to send files from the file system, tntnet has a standard
component for that. It is called static@tntnet. You just need to add a url
mapping for that.

The component does almost that, what the code from the previous example does.
Additionally it sends modification dates and sends http code 304 (not modified)
if it makes sense. This is needed to enable caching of files in the browser.

Also it uses some optimizations like the sendfile system call if available and
suitable.

For completeness here is a url mapping for this component:

    <mapping>
      <url>^/(.*)</url>
      <target>static@tntnet</target>
      <pathinfo>/var/www/htdocs/$1</pathinfo>
    </mapping>

This makes the content of the directory /var/www/htdocs available to the world.
But keep in mind, that you can't list directory contents with this component.
There is also no standard component for this.

As a side effect, static@tntnet looks for files with a .gz extension. If the
client accepts gzip compression and the corresponding file with a .gz extension
is found, that one is sent.

If the file is not found in the file system, static@tntnet sends a DECLINED just like in the custom component above, so it is also possible to chain multiple search paths for files.

Multibinary components
----------------------

Let's return to the first solution. We mentioned the significant overhead for
each component. If you have many small files, the overhead gets larger and
larger. The solution is a multibinary component. This is a component, which has
multiple static pages. We get only once the 8k overhead. As a side effect
compile time is significantly reduced.

To create multibinary components, we pass -bb to ecppc (or -b -b) and a list of
file names, which to include into the component. Normally you want to pass a
component name with -n to ecppc. By default ecppc uses the name "image". Useful
is also to disable stripping paths from the file names with -p. Normally ecppc
takes only the base name of the files.

What we need is a suitable mapping. We have to tell the new component, which
file to send. This is done using the path info, which we can pass from the url
mapping after the component name. Lets looks at an example.

We have 2 files p/foo.html and p/bar.html and want to create a multibinary
component named static. This is done using the command:

    ecppc -bb -n static -p p/foo.html p/bar.html

Now we have a static.cpp in the current directory. Next we create the mapping.
Our shared library is called myapp. We want to send the component foo.html
using the path /myfiles/p/foo.html and bar.html using /myfiles/p/bar.html:

    <mapping>
      <url>^/myfiles/(.*)</url>
      <target>static@myapp</target>
      <pathinfo>$1</pathinfo>
    </mapping>

And we are done.

Conclusion
----------

You have to choose from the 4 presented solutions, which one fits best to your
problem.

Just compiling each file into a component is easy and done quickly.

Most larger applications have a bunch of static files, which are best placed
into a multibinary component.

For less static data you may decide to put the files into the file system using
static@tntnet or in the database using your own custom code.

Chaining is a interesting technique. You may choose to put the static files on
development time into the file system first, so you can quickly test them. If
you are happy with the content, just compile the file into your application and
remove the file from the file system.
