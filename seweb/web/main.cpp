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

#include <iostream>
#include <tnt/tntnet.h>
#include <cxxtools/log.h>
#include <seweb/configuration.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    seweb::Configuration config = seweb::Configuration::get();

    tnt::Tntnet app;

    // map jquery/foo/bar/ to foo/bar/index.html in jquery lib
    app.mapUrl("^/jquery/(.*)/$", "jquery").setPathInfo("$1/index.html");
    app.mapUrl("^/jquery/$", "jquery").setPathInfo("index.html");

    // map jquery/foo/bar to foo/bar in jquery lib
    app.mapUrl("^/jquery/(.*)$", "jquery").setPathInfo("$1");

    // map / to index file with layout
    app.mapUrl("^/$", "htmlmain").setPathInfo("index");

    // add layout page htmlmain to all *.html files
    app.mapUrl("^/(.*)\\.html$", "htmlmain").setPathInfo("$1");

    // map foo.json to fooJson
    app.mapUrl("^/(.*).json$", "$1Json");

    // map foo.xml to fooXml
    app.mapUrl("^/(.*).xml$", "$1Xml");

    app.listen(config.httpip(), config.httpport());

    std::cout << "http service listening on " << config.httpip() << ':' << config.httpport() << std::endl;

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

