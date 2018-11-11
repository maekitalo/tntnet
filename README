Tntnet
======

Tntnet is a web application server for web applications written in C++.

You can write a Web-page with HTML and with special tags you embed
C++-code into the page for active contents. These pages, called components are
compiled into C++-classes with the ecpp-compilier "ecppc", then compiled into
objectcode and linked into a process or shared library.

This shared library is loaded by the webserver "tntnet" on request and
executed.

The ecpp-compiler can create also C++-classes from static files of any
type, e.g. you can compile a jpeg-image into the library. So the
whole webapplication is a single library.

The application runs native, so they are very fast and compact.

Features supported include: cookies, HTTP-upload, automatic request-parameter
parsing and conversion, automatic sessionmanagement, scoped variables
(application, request and session), internationalisation, keep-alive.

Logging is done through cxxtools.

Tntnet is fully multithreaded, so it scales well on multiprocessor machines.
It uses a dynamic pool of workerthreads, which answers requests from
http-clients.

Installation 
============

To install tntnet, you need cxxtools (http://www.tntnet.org/).

You can find generic installation instructions in the file INSTALL.

Quick start
===========

To create a simple application run "tntnet-project hello".
This creates a directory "hello" with a simple project and prints
a short message, how to run the application.

There are some demo-applications you can try in _demos_. To run the demos
without installing tntnet, change to the directory of the demo and run tntnet
from the directory framework/runtime:
    cd hello
    ../../framework/runtime/tntnet

Tntnet listens on port 8000. Start your browser and navigate to:
    http://localhost:8000/

Documentation is provided in man pages and some documents found in the doc
directory.

Ssl
===

Tntnet supports ssl. It must be configured in tntnet.xml or using the api.
For ssl a certificate is needed. To create a self signed certificate run:

    openssl req -x509 -nodes -days 1095 -utf8 -subj "/C=DE/L=Your Town/CN=$(hostname -f)/O=Your Organization/OU=Your unit" -newkey rsa:2048 -keyout tntnet.pem -out tntnet.pem

Client side certificates can be used to restrict access to tntnet.
