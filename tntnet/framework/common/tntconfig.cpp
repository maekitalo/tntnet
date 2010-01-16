/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <tnt/tntconfig.h>
#include <tnt/util.h>
#include <stdexcept>
#include <fstream>
#include <stack>
#include <cctype>
#include <cxxtools/multifstream.h>

namespace tnt
{
  //////////////////////////////////////////////////////////////////////
  // ConfigParser
  //
  void ConfigParser::parse(char ch)
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
  // TntconfigParser
  //
  class TntconfigParser : public ConfigParser
  {
      typedef std::stack<std::istream*> istreams_type;
      istreams_type istreams;

      Tntconfig& config;

      bool checkInclude(const std::string& key, const params_type& params);

    protected:
      virtual void onLine(const std::string& key, const params_type& value);

    public:
      TntconfigParser(Tntconfig& config_)
        : config(config_)
        { }

      void parse(std::istream& in);
  };

  bool TntconfigParser::checkInclude(const std::string& key, const params_type& params)
  {
    if (key == "include" && params.size() == 1)
    {
      std::istream* inp = new cxxtools::multi_ifstream(params[0].c_str());
      if (!*inp)
      {
        delete inp;
        throwRuntimeError("cannot open include file " + params[0]);
      }
      else if (istreams.size() > 5)
        throwRuntimeError("too many include-levels");

      istreams.push(inp);
      return true;
    }
    else
      return false;
  }

  void TntconfigParser::onLine(const std::string& key, const params_type& params)
  {
    if (!checkInclude(key, params))
      config.setConfigValue(key, params);
  }

  void TntconfigParser::parse(std::istream& in)
  {
    char ch;

    istreams.push(&in);

    try
    {
      while (istreams.size() > 0)
      {
        while (istreams.top()->get(ch))
          ConfigParser::parse(ch);
        ConfigParser::parse('\n');

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
      throwRuntimeError("parse error while reading config");
  }

  //////////////////////////////////////////////////////////////////////
  // Tntconfig
  //
  void Tntconfig::load(const char* configfile)
  {
    std::ifstream in(configfile);
    if (!in)
    {
      std::string msg;
      msg = "error opening ";
      msg += configfile;
      throwRuntimeError(msg);
    }
    load(in);
  }

  void Tntconfig::load(std::istream& in)
  {
    TntconfigParser parser(*this);
    parser.parse(in);
  }

  void Tntconfig::setConfigValue(const std::string& key, const params_type& params)
  {
    config_entries.push_back(config_entry_type());
    config_entries[config_entries.size() - 1].key = key;
    config_entries[config_entries.size() - 1].params = params;
  }

  Tntconfig::params_type Tntconfig::getConfigValue(
       const std::string& key,
       const params_type& def) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
      if (it->key == key)
        return it->params;
    return def;
  }

  void Tntconfig::getConfigValues(
       const std::string& key,
       config_entries_type& ret) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
      if (it->key == key)
        ret.push_back(*it);
  }

  std::string Tntconfig::getValue(
       const std::string& key,
       const params_type::value_type& def) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
    {
      if (it->key == key && it->params.size() > 0)
        return it->params[0];
    }

    return def;
  }

  bool Tntconfig::hasValue(const std::string& key) const
  {
    for (config_entries_type::const_iterator it = config_entries.begin();
         it != config_entries.end(); ++it)
    {
      if (it->key == key && it->params.size() > 0)
        return true;
    }

    return false;
  }

}
