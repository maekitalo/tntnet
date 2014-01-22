Using jQuery in Tntnet applications
===================================

Introduction
------------

[jQuery](http://jquery.com) is a fast, small, and feature-rich JavaScript
library. It makes things like HTML document traversal and manipulation, event
handling, animation, and Ajax much simpler with an easy-to-use API that works
across a multitude of browsers. With a combination of versatility and
extensibility, jQuery has changed the way that millions of people write
JavaScript.

To integrate jQuery in a web application, the jQuery library must be made
available to the web application. This is normally done by copying the jQuery to
the web space of a web server. From the point of view of the web server it is
just a static file.

Since this is valid for many javascipt libraries, this same howto can be used to
integrate other javascript libraries as well.

Make jQuery available to the application
----------------------------------------

When using jQuery in a web application, jQuery becomes a essential part of the
application. In tntnet all those parts are typically compiled into the
application, so that the resulting binary will be self contained.

There are more than one way to do it in tntnet but lets concentrate to the
simplest one first.

The `ecppc` compiler has a binary mode, which do not interpret any tags in the
source. This is used to compile static files like the jquery.js file into C++
code. The flag `-b` must be passed to `ecppc`. The command is:

    ecppc -b jquery.js

This generates a jquery.cpp, which can be compiled and linked into your web
application. The code must be compiled and linked into your application.

The next step is to make it available to the browser. For this a suitable
mapping from url to the component name is needed. The name of the resulting
component is the basename of our file, so it is _jquery_ here.

When you for example compile and link the resulting code into a library named
_myapp_ the full component name for the mapping is _jquery@myapp_.

The generated applications use a standard mapping:

    <mapping>
      <target>$1@myapp</target>
      <url>^/([^.]+)(\..+)?</url>
    </mapping>

This is fine here. The url _/jquery.js_ is mapped to our component and hence
delivers the jquery library to the browser under this url.

To use it in a html page, you have to add this into the `<head>` section of the
page:

    <script type="text/javascript" src="/jquery.js"></script>

And we are done.


TODO full html example

Adding jQuery UI
----------------

The [jQuery UI](http://jqueryui.com) library adds many widgets and effects to
the jQuery library like date pickers or progress bars.

Compared to jQuery, the jQuery UI is not just one file but a whole directory
structure. This makes it a little more complicated to compile it into your
application. But once understood it is not really difficult.

Since there are so many files, it is much easier and also cheaper to put all the
files into one single component. This is done using the multibinary feature of
`ecppc` by passing the option `-bb`. This feature can combine multiple files
into a single tntnet component.  To access a single file in that component the
path info must be set in the mapping. This sounds complicated but it isn't.

First we create a component from our files. We download jQuery UI from the web
site and unpack the files in our project first. Lets assume, our jQuery UI files
can be found in our project directory under the path _jquery-ui-1.10.3_. The multibinary
feature of `ecppc` expects a list of file names as command line parameters. You
should set a suitable component name using the option `-n` also since by default
the name is just `image`, which will result in duplicate component names if you
have multiple multibinary components in your application.

The simplest way is to use the command:

    ecppc -bb -n jqueryui -p $(find jquery-ui-1.10.3 -type f)

Which uses the shell to find all files in our jQuery UI distribution and passes
those to `ecppc`. The option `-p` is important here also since by default the
path is stripped from our files when generating urls, which must not be done
for jQuery UI.

Since the output file name is built from the component name, the resulting file
is named jqueryui.cpp. This can be again compiled and linked into our
application.

Note that we need to pass the full path name including _jquery-ui-1.10.3_ to the
_jqueryui_ component in the mapping. The standard mapping is not suitable any
more. We need our own mapping. We generate a mapping, where we strip or
directory prefix away:

    <mapping>
      <target>jqueryui@myapp</target>
      <url>^/(.*)?</url>
      <pathinfo>jquery-ui-1.10.3/$1</pathinfo>
    </mapping>

Now we are ready to use jQuery UI in our web application. As a example, we
create a text field with a date picker.

For that we need to load some javascript files and on css file. We add this to
our `<head>`:

    <script type="text/javascript" src="/jquery.js"></script>
    <script type="text/javascript" src="/ui/ui/jquery.ui.core.js"></script>
    <script type="text/javascript" src="/ui/ui/jquery.ui.datepicker.js"></script>
    <link rel="stylesheet" href="/ui/themes/base/jquery-ui.css">

Now we are ready to add a datepicker to our text fields by using the
`datepicker` function of jquery-ui on initialization:

    <script>
      $(function() {
        $( "#datepicker" ).datepicker();
      });
    </script>

We assume we have a input element in our html page somewhere:

    <p>Date: <input type="text" id="datepicker"></p>

For details about the datepicker you may want to read the manual of jQuery UI.

For completeness, here is the full page:

    <!doctype html>
    <html>
    <head>
      <title>ecpp application jdate</title>
      <script type="text/javascript" src="/jquery.js"></script>
      <script type="text/javascript" src="/ui/ui/jquery.ui.core.js"></script>
      <script type="text/javascript" src="/ui/ui/jquery.ui.datepicker.js"></script>
      <link rel="stylesheet" href="/ui/themes/base/jquery-ui.css">

      <script>
        $(function() {
          $( "#datepicker" ).datepicker();
        });
      </script>

    </head>
    <body>
      <h1>jQuery UI datepicker demo</h1>
      <p>Date: <input type="text" id="datepicker"></p>
    </body>
    </html>

Using Ajax
----------

Ajax is an acronym for Asyncronous JavaScript and XML. Despite the name the use
of XML is not required. Today most Ajax requests do not actually use XML at all.
Frequently JSON or just HTML fragments are sent from the server as a reply of
asynchronous requests.

The jQuery library makes it easy to work with those asynchronous requests. And
since for Ajax applications it is typical, that many small requests are sent to
the server, it is recommended to uses a quick server side framework like tntnet.

So lets look for a example how to work with Ajax with jQuery and tntnet. We
create a page with 2 buttons. Clicking on a button will load a html fragment
from the server.

We already have integrated jQuery into our application. Now we add a page, say
`mainpage.ecpp` and 2 html fragments `page1.ecpp` and `page2.ecpp`. The
`mainpage.ecpp` is here:

    <!doctype html>
    <html>
    <head>
      <title>jQuery demo application</title>
      <script type="text/javascript" src="/jquery.js"></script>

      <script>
        $(function() {
          $("#page1").click( function() {
            $("#content").load("page1");
          });
          $("#page2").click( function() {
            $("#content").load("page2");
          });
        });
      </script>

    </head>
    <body>
      <h1>jQuery Ajax demo</h1>
      <input type="button" id="page1" value="load page 1">
      <input type="button" id="page2" value="load page 2">
      <div id="content"></div>
    </body>
    </html>

So actually a very simple jQuery application. Clicking on one of the buttons
will load the subpage into our content div. The `page1.ecpp` and `page2.ecpp`
may contain ecpp code including dynamic content, which is loaded into the div
without reloading the whole page.

The special here regarding tntnet is that there is nothing special here. It just
gives us the chance to load dynamic html fragments from the server.

Json
----

Instead of loading html fragments, it is sometimes more convenient to load just
the data and process data on the client side in JavaScript.

In tntnet server side processing is done using C++. Data processing results in
C++ objects or structs. We need a facility to transfer those C++ objects to the
JavaScript world and make them accessible there. We cannot send C++ objects over
the wire and neither can use C++ objects in JavaScript. So we need a way to
convert those C++ objects into JavaScript.

Luckily we have JSON support in the serialization framework of cxxtools which
greatly simplifies that.

On the client side we use the function `jQuery.getJSON()`.

To demonstrate the use, we need to write 2 components. One component is the
page, which displays the screen and requests the data and the other component
processes that request and sends the result back in JSON format.

In our example we create a screen, which shows a product in a table with buttons
to request the next and previous product. For simplicity we omit any kind of
error handling.

So here is our screen, which shows the products:

    <!doctype html>
    <html>
    <head>
      <title>Products</title>
      <script type="text/javascript" src="/jquery.js"></script>

      <script>
        var currentRec = 0;
        function loadData() {
          $.getJSON('getproduct',
            {
              recno: currentRec
            },
            function (product) {
              $("#name").html(product.name);
              $("#price").html(product.price);
            });
        }

        $(function() {
          $("#prev").click(function () {
            if (currentRec > 0)
            {
              --currentRec;
              loadData();
            }
          });

          $("#next").click(function () {
            ++currentRec;
            loadData();
          });

          loadData();  // initialize screen by loading the first record
        });
      </script>

    </head>
    <body>
      <h1>Products</h1>
      <table>
       <tr>
        <th>Name</th>
        <td><span id="name"></span></td>
       <tr>
       <tr>
        <th>Price</th>
        <td><span id="price"></span></td>
       </tr>
      <table>
      <input type="button" id="prev" value="previous">
      <input type="button" id="next" value="next">
    </body>
    </html>

The html part shows a simple table with one product. In the JavaScript part we
define a variable `currentRec` which holds the current record number and a
function `loadData` which loads the current record into the screen using JSON.

We implement and bind appropriate JavaScript functions for the two buttons to
load the next or previous records.

The function `$.getJSON` expects a JSON result and calls the callback function
when the reply returns with success. In that callback we get the JSON stucture
already parsed. So it is very easy to access the members of our product. Note
that we pass the requested record number as `recno` to the server.

In the next step we need to implement the server side component, which returns
the selected record.

Here is the code:

    <%args>
    unsigned recno;
    </%args>
    <%pre>

    #include <cxxtools/serializationinfo.h>
    #include <cxxtools/jsonserializer.h>

    // we define a data structure:
    struct Product
    {
      std::string name;
      double price;
    };

    // to convert the struct to JSON we need to make it serializable by defining a
    // serialization operator for that:
    inline void operator<<= (cxxtools::SerializationInfo& si, const Product& product)
    {
      si.addMember("name") <<= product.name;
      si.addMember("price") <<= product.price;
    }

    // This is our "database"
    Product products[] = {
      { "Milk", 1.5 },
      { "Honey", 3.2 }
    };

    const unsigned productCount = sizeof(products)/sizeof(Product);

    </%pre>
    <%cpp>

      if (recno < productCount)
      {
        // convert the selected record to JSON using the JSON serializer of cxxtools:
        cxxtools::JsonSerializer serializer(reply.out());
        serializer.serialize(products[recno]);
      }

    </%cpp>

Here we define a C++ structure and make it serializable by defining the
serialization operator. In a real world project we would implement that in
separate C++ files and headers but here we just put it inline into our
component.

We simulate a database using just a static array of products.

Using `cxxtools::JsonSerializer` we convert the requested product into JSON
format, which can be easily processed on the client side using JavaScript as
seen above.

Conclusion
----------

Using jQuery with tntnet it is easy to create powerful and highly responsive
applications. Tntnet gives the developer the power and flexibility on the server
side. jQuery makes it easy on the client side.

Using the serialization it is even easy to use C++ data structures in JavaScript
code.

The demonstration code can be found at [github](https://github.com/maekitalo/jqtnt).
