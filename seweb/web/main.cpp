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
    app.mapUrl("^/$", "htmlmain").setPathInfo("index");
    app.mapUrl("^/(.*)\\.html$", "htmlmain").setPathInfo("$1");
    app.mapUrl("^/(.*).json$", "$1Json");
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

