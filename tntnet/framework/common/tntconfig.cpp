/* tntconfig.cpp
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

#include <tnt/tntconfig.h>
#include <stdexcept>
#include <fstream>
#include <stack>
#include <sstream>
#include <cxxtools/multifstream.h>

namespace tnt
{
  //////////////////////////////////////////////////////////////////////
  // config_parser
  //
  void config_parser::parse(char ch)
  {
    switch(state)
    {
      case state_start:
        if (ch == '#')
          state = state_comment;
        else if (!std::isspace(ch))
        {
          current_cmd = ch;
          state = state_cmd;
        }
        break;

      case state_cmd:
        if (ch == '\n')
        {
          onLine(current_cmd, current_params);
          current_cmd.clear();
          current_params.clear();
          state = state_start;
        }
        else if (ch == '#')
        {
          onLine(current_cmd, current_params);
          current_cmd.clear();
          current_params.clear();
          state = state_comment;
        }
        else if (std::isspace(ch))
          state = state_args;
        else
          current_cmd += ch;
        break;

      case state_args:
        if (ch == '\n' || ch == '#')
        {
          onLine(current_cmd, current_params);
          current_cmd.clear();
          current_params.clear();

          state = ch == '\n' ? state_start : state_comment;
        }
        else if (ch == '\\')
          state = state_args_esc;
        else if (ch == '"')
          state = state_qstring;
        else if (!std::isspace(ch))
        {
          current_token = ch;
          state = state_token;
        }
        break;

      case state_args_esc:
        if (ch == '\n')
          state = state_args;
        else
        {
          current_token = ch;
          state = state_token;
        }
        break;

      case state_token:
        if (ch == '\n' || ch == '#')
        {
          current_params.push_back(current_token);
          current_token.clear();

          onLine(current_cmd, current_params);
          current_cmd.clear();
          current_params.clear();

          state = ch == '\n' ? state_start : state_cmd;
        }
        else if (std::isspace(ch))
        {
          current_params.push_back(current_token);
          current_token.clear();
          state = state_args;
        }
        else
          current_token += ch;
        break;

      case state_qstring:
        if (ch == '"')
        {
          current_params.push_back(current_token);
          current_token.clear();
          state = state_args;
        }
        else if (ch == '\\')
          state = state_qstring_esc;
        else
          current_token += ch;
        break;

      case state_qstring_esc:
        current_token += ch;
        state = state_qstring;
        break;

      case state_comment:
        if (ch == '\n')
          state = state_start;
        break;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // tntconfig_parser
  //
  class tntconfig_parser : public config_parser
  {
      typedef std::stack<std::istream*> istreams_type;
      istreams_type istreams;

      tntconfig& config;

      bool checkInclude(const std::string& key, const params_type& params);

    protected:
      virtual void onLine(const std::string& key, const params_type& value);

    public:
      tntconfig_parser(tntconfig& config_)
        : config(config_)
        { }

      void parse(std::istream& in);
  };

  bool tntconfig_parser::checkInclude(const std::string& key, const params_type& params)
  {
    if (key == "include" && params.size() == 1)
    {
      std::istream* inp = new cxxtools::multi_ifstream(params[0].c_str());
      if (!*inp)
      {
        delete inp;
        std::ostringstream msg;
        throw std::runtime_error("cannot open include file " + params[0]);
      }
      else if (istreams.size() > 5)
        throw std::runtime_error("too many include-levels");

      istreams.push(inp);
      return true;
    }
    else
      return false;
  }

  void tntconfig_parser::onLine(const std::string& key, const params_type& params)
  {
    if (!checkInclude(key, params))
      config.setConfigValue(key, params);
  }

  void tntconfig_parser::parse(std::istream& in)
  {
    char ch;

    istreams.push(&in);

    try
    {
      while (istreams.size() > 0)
      {
        while (istreams.top()->get(ch))
          config_parser::parse(ch);
        config_parser::parse('\n');

        if (istreams.size() > 1)
          delete istreams.top();

        istreams.pop();
      }
    }
    catch(const std::exception &)
    {
      while (istreams.size() > 1)
      {
        delete istreams.top();
        istreams.pop();
      }
      throw;
    }

    if (state != state_start)
      throw std::runtime_error("parse error while reading config");
  }

  //////////////////////////////////////////////////////////////////////
  // tntconfig
  //
  void tntconfig::load(const char* configfile)
  {
    std::ifstream in(configfile);
    if (!in)
    {
      std::string msg;
      msg = "error opening ";
      msg += configfile;
      throw std::runtime_error(msg);
    }
    load(in);
  }

  void tntconfig::load(std::istream& in)
  {
    tntconfig_parser parser(*this);
    parser.parse(in);
  }

  void tntconfig::setConfigValue(const std::string& key, const params_type& params)
  {
    config_entries.push_back(config_entry_type());
    config_entries[config_entries.size() - 1].key = key;
    config_entries[config_entries.size() - 1].params = params;
  }

  tntconfig::params_type tntconfig::getConfigValue(
       const std::string& key,
       const params_type& def) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
      if (it->key == key)
        return it->params;
    return def;
  }

  void tntconfig::getConfigValues(
       const std::string& key,
       config_entries_type& ret) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
      if (it->key == key)
        ret.push_back(*it);
  }

  std::string tntconfig::getValue(
       const std::string& key,
       const params_type::value_type& def) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
      if (it->key == key && it->params.size() > 0)
        return it->params[0];
    return def;
  }
}
