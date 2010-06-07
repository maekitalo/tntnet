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
#include <cxxtools/http/server.h>
#include <seweb/service/veranstaltung.h>
#include <seweb/configuration.h>
#include <cxxtools/log.h>
#include <cxxtools/eventloop.h>

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    seweb::Configuration config = seweb::Configuration::get();

    cxxtools::EventLoop eventLoop;
    cxxtools::http::Server server(eventLoop, config.rpcip(), config.rpcport());

    seweb::service::Veranstaltung veranstaltung;
    server.addService("/veranstaltung", veranstaltung);

    std::cout << "xmlrpc service listening on " << config.rpcip() << ':' << config.rpcport() << std::endl;

    eventLoop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

