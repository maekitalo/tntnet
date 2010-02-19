/*
 * Copyright (C) 2010 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

/**
 This demo shows how to create a standalone application server just with
 libtntnet.

 Normally applications are compiled into shared libraries and loaded at
 runtime. Configuration, especially the mapping from url to component, is done
 with tntnet.conf.

 As an alternative you may write your own small main function and compile the
 components into a program. There is no configuration file tntnet.conf read any
 more but you have to code your mappings into the program. The advantage is,
 that there is no configuration file needed. Since the mappings are often a
 fundamental part of your application, it is better to hard code it into the
 application.

 */
#include <iostream>
#include <tnt/tntnet.h>
#include <tnt/configurator.h>
#include <cxxtools/log.h>

int main(int argc, char* argv[])
{
  try
  {
    // initialize logging - this is optional. If log_init is not called, no
    // logging is done
    log_init("tntnet.properties");

    // instantiate the tnt::Tntnet application class
    tnt::Tntnet app;

    app.setAppName("calc");

    // set your settings
    app.mapUrl("^/$", "calc");
    app.mapUrl("^/([^.]+)(\..+)?", "$1");

    // set more settings; the settings from tntnet.conf are spread throughout
    // different classes. It is quite difficult to know, where to find the right
    // place, where to set the settings. The helper class tnt::Configurator
    // makes it easy though.
    tnt::Configurator configurator(app);
    configurator.setMinThreads(3);        // MinThreads in tntnet.conf
    configurator.setSessionTimeout(600);  // SessionTimeout in tntnet.conf

    // run the application
    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

