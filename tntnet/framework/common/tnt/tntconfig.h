/* tnt/tntconfig.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNTCONFIG_H
#define TNTCONFIG_H

#include <string>
#include <vector>
#include <iostream>

namespace tnt
{
  class tntconfig
  {
    public:
      typedef std::vector<std::string> config_value_type;
      typedef std::string name_type;
      typedef struct {
          name_type key;
          config_value_type values;
          void clear()
          {
            key.clear();
            values.clear();
          }
        } config_entry_type;
      typedef std::vector<config_entry_type> config_values_type;

    private:
      config_values_type config_values;

    public:
      tntconfig()
      { }
      void load(const char* configfile);
      void load(std::istream& in);

      const config_values_type& getConfigValues() const
      { return config_values; }

      config_value_type getConfigValue(
           const std::string& key,
           const config_value_type& def = config_value_type()) const;

      void getConfigValues(
           const std::string& key,
           config_values_type& ret) const;

      config_value_type::value_type getSingleValue(
           const std::string& key,
           const config_value_type::value_type& def = config_value_type::value_type()) const;
  };
}

#endif // TNTCONFIG_H

