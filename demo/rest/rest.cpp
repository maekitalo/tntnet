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

#include <iostream>
#include <tnt/tntnet.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    // read listen ip from command line switch -l with default to empty, which
    // means: all interfaces
    cxxtools::Arg<std::string> ip(argc, argv, 'l');

    // read listen port from command line switch -p with default to 8000
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

    // initialize logging - this is optional. If log_init is not called,
    // logging is not enabled
    log_init("tntnet.properties");

    // instantiate the tnt::Tntnet application class
    tnt::Tntnet app;

    // optionally set the application name
    app.setAppName("rest");

    // set up mappings

    // Map method GET to "getvalue" and pass url as argument "key".
    app.mapUrl("^/(.*)$", "getvalue")
       .setMethod("^GET$")
       .setArg("key", "$1");

    // Map method PUT to "putvalue" and pass url as argument "key".
    // The value is read from the http body.
    app.mapUrl("^/(.*)$", "putvalue")
       .setMethod("^PUT$")
       .setArg("key", "$1");

    // Map method DELETE to "delvalue" and pass url as argument "key".
    app.mapUrl("^/(.*)$", "delvalue")
       .setMethod("^DELETE$")
       .setArg("key", "$1");

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
