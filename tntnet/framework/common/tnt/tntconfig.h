/* tnt/tntconfig.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#ifndef TNT_TNTCONFIG_H
#define TNT_TNTCONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace tnt
{
  class config_parser
  {
    public:
      typedef std::vector<std::string> params_type;

      enum state_type {
        state_start,
        state_cmd,
        state_args,
        state_args_esc,
        state_token,
        state_qstring,
        state_qstring_esc,
        state_comment };

    private:
      std::string current_cmd;
      params_type current_params;
      std::string current_token;

    protected:
      state_type state;
      virtual void onLine(const std::string& key, const params_type& value) = 0;

    public:
      config_parser()
        : state(state_start)
        { }
      virtual ~config_parser()
        { }

      void parse(char ch);
  };

  class tntconfig
  {
    public:
      typedef config_parser::params_type params_type;
      typedef std::string name_type;
      struct config_entry_type {
          name_type key;
          params_type params;
        };
      typedef std::vector<config_entry_type> config_entries_type;

    private:
      config_entries_type config_entries;

    public:
      tntconfig()
      { }
      void load(const char* configfile);
      void load(std::istream& in);

      const config_entries_type& getConfigValues() const
      { return config_entries; }

      void setConfigValue(const std::string& key, const params_type& value);

      params_type getConfigValue(
           const std::string& key,
           const params_type& def = params_type()) const;

      void getConfigValues(
           const std::string& key,
           config_entries_type& ret) const;

      std::string getValue(
           const std::string& key,
           const std::string& def = std::string()) const;
      template <typename T>
        T getValue(const std::string& key, const T& def) const
        {
          T ret;
          std::istringstream v(getValue(key));
          return (v >> ret) ? ret : def;
        }

      std::string getValue(const std::string& key, const char* def) const
      {
        return getValue(key, std::string(def));
      }

      bool hasValue(const std::string& key) const;
  };
}

#endif // TNT_TNTCONFIG_H

