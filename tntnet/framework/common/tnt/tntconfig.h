/* tnt/tntconfig.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNT_TNTCONFIG_H
#define TNT_TNTCONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace tnt
{
  class ConfigParser
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
      ConfigParser()
        : state(state_start)
        { }
      virtual ~ConfigParser()
        { }

      void parse(char ch);
  };

  class Tntconfig
  {
    public:
      typedef ConfigParser::params_type params_type;
      typedef std::string name_type;
      struct config_entry_type {
          name_type key;
          params_type params;
        };
      typedef std::vector<config_entry_type> config_entries_type;

    private:
      config_entries_type config_entries;

    public:
      Tntconfig()
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
      bool getBoolValue(const std::string& key, bool def) const
        {
          std::string v = getValue(key);
          if (v.empty())
            return def;
          char ch = v.at(0);
          return ch == '1' || ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y';
        }

      std::string getValue(const std::string& key, const char* def) const
      {
        return getValue(key, std::string(def));
      }

      bool hasValue(const std::string& key) const;
  };
}

#endif // TNT_TNTCONFIG_H

