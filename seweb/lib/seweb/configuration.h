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

#ifndef SEWEB_CONFIGURATION_H
#define SEWEB_CONFIGURATION_H

#include <string>

namespace cxxtools
{
  class SerializationInfo;
}

namespace seweb
{
  class Configuration
  {
      friend void operator>>= (const cxxtools::SerializationInfo& si, Configuration& co);
      std::string _dburl;
      std::string _rpcip;
      unsigned short _rpcport;
      std::string _httpip;
      unsigned short _httpport;

    public:
      static const Configuration& get();

      const std::string& dburl() const
      { return _dburl; }

      const std::string& rpcip() const
      { return _rpcip; }

      unsigned short rpcport() const
      { return _rpcport; }

      const std::string& httpip() const
      { return _httpip; }

      unsigned short httpport() const
      { return _httpport; }

  };
}

#endif // SEWEB_CONFIGURATION_H
