/*
 * Copyright (C) 2016 Tommi Maekitalo
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

/**
 This demo shows how to create a standalone application server just with
 libtntnet.

 Normally applications are compiled into shared libraries and loaded at
 runtime. Configuration, especially the mapping from url to component, is done
 with tntnet.xml.

 As an alternative you may write your own small main function and compile the
 components into a program. There is no configuration file tntnet.xml read any
 more but you have to code your mappings into the program. The advantage is,
 that there is no configuration file needed. Since the mappings are often a
 fundamental part of your application, it is better to hard code it into the
 application.

 */
#include <iostream>
#include <tnt/tntnet.h>
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
    try
    {
        // read listen ip from command line switch -l with default to empty, which means: all interfaces
        cxxtools::Arg<std::string> ip(argc, argv, 'l');

        // read listen port from command line switch -p with default to 8000
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

        // initialize logging - this is optional. If log_init is not called, no
        // logging is done
        log_init("tntnet.properties");

        // instantiate the tnt::Tntnet application class
        tnt::Tntnet app;

        // optionally set the application name
        app.setAppName("friends");

        // set up mappings

        // map "/object/id" to "object/get" with parameter id
        app.mapUrl("^/(..*)/([0-9][0-9]*)$", "$1/get")
           .setMethod("^GET$")
           .setArg("id", "$2");

        // map "/object/lastname/firstname" to "object/getbyname" with parameters lastname and firstname
        app.mapUrl("^/(..*)/(.*)/(.*)$", "$1/getbyname")
           .setMethod("^GET$")
           .setArg("lastname", "$2")
           .setArg("firstname", "$3");

        // map "/object/" to "object/all"
        app.mapUrl("^/(..*)/$", "$1/all")
           .setMethod("^GET$");

        // map "/object" with method POST to "object/insert"
        app.mapUrl("^/(..*)$", "$1/insert")
           .setMethod("^POST$");

        // map "/object/id" with method PUT to "object/update" with parameter id
        app.mapUrl("^/(..*)/([0-9][0-9]*)$", "$1/update")
           .setMethod("^PUT$") 
           .setArg("id", "$2");

        // map "/object/id" with method DELETE to "object/delete"
        app.mapUrl("^/(..*)/([0-9][0-9]*)$", "$1/delete")
           .setMethod("^DELETE$")
           .setArg("id", "$2");

        // configure listener
        app.listen(ip, port);  // note that a empty ip address tells tntnet to listen on all local interfaces

        // run the application
        app.run();
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
}
