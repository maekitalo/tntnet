/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <configuration.h>
#include <cxxtools/properties.h>
#include <fstream>

void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config)
{
  si.getMember("listen.ip", config._listenIp);  // listen.ip is optional - default is to listen on all local interfaces
  si.getMember("listen.port") >>= config._listenPort;
  si.getMember("sessiontimeout", config._sessionTimeout);  // sessiontimeout is optional
  si.getMember("dburl", config._dburl);
  config._loggingConfiguration = si;
}

Configuration::Configuration()
    : _listenPort(0),
      _sessionTimeout(0)
{ }

void Configuration::readConfiguration(const std::string& file)
{
  std::ifstream in(file.c_str());
  in >> cxxtools::Properties(Configuration::it());
}

Configuration& Configuration::it()
{
  static Configuration theConfiguration;
  return theConfiguration;
}
