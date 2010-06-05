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

#include <seweb/configuration.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/mutex.h>
#include <cxxtools/xml/xmldeserializer.h>
#include <cxxtools/log.h>
#include <fstream>

log_define("seweb.configuration")

namespace seweb
{
void operator>>= (const cxxtools::SerializationInfo& si, Configuration& co)
{
  si.getMember("dburl") >>= co._dburl;
  si.getMember("rpcport") >>= co._rpcport;
  si.getMember("rpcip") >>= co._rpcip;
  si.getMember("httpport") >>= co._httpport;
  si.getMember("httpip") >>= co._httpip;
}

const Configuration& Configuration::get()
{
  static Configuration configuration;
  static bool initialized = false;
  static cxxtools::Mutex mutex;
  cxxtools::MutexLock lock(mutex);
  if (!initialized)
  {
#ifdef SYSCONFDIR
    std::string sysconfdir = SYSCONFDIR;
#else
#warning SYSCONFDIR not set
    std::string sysconfdir = "/etc";
#endif
    log_debug("read configuration " << sysconfdir << "/seweb.xml");
    std::string fname = sysconfdir + "/seweb.xml";
    std::ifstream in(fname.c_str());
    if (!in)
      throw std::runtime_error("failed to read configuration from \"" + fname + '"');

    try
    {
      cxxtools::xml::XmlDeserializer deserializer(in);

      deserializer.deserialize(configuration);
      initialized = true;
    }
    catch (const std::exception& e)
    {
      throw std::runtime_error("error reading configuration file \"" + fname + "\": " + e.what());
    }
  }

  return configuration;
}

}
