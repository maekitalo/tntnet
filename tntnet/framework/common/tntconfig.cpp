////////////////////////////////////////////////////////////////////////
// tntconfig.cpp
//

#include <tnt/tntconfig.h>
#include <stdexcept>
#include <fstream>
#include <stack>
#include <sstream>
#include <cxxtools/multifstream.h>

namespace tnt
{

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

  typedef std::stack<std::istream*> istreams_type;

  static bool checkInclude(tntconfig::config_entry_type& ce, istreams_type& include_stack)
  {
    if (ce.key == "include" && ce.values.size() == 1)
    {
      std::istream* inptr = new multi_ifstream(ce.values[0].c_str());
      if (!inptr && !*inptr)
      {
        delete inptr;
        std::ostringstream msg;
        throw std::runtime_error("cannot open include file " + ce.values[0]);
      }
      else if (include_stack.size() > 5)
        throw std::runtime_error("too many include-levels");

      include_stack.push(inptr);
      return true;
    }
    else
      return false;
  }

  void tntconfig::load(std::istream& in)
  {
    enum state_type {
      state_start,
      state_cmd,
      state_args,
      state_args_esc,
      state_token,
      state_qstring,
      state_qstring_esc,
      state_comment };

    state_type state = state_start;
    char ch;
    config_entry_type config_entry;
    std::string token;

    istreams_type istreams;
    std::istream* inptr = &in;
    istreams.push(inptr);

    try
    {
      while (inptr)
      {
        while (inptr->get(ch))
        {
          switch(state)
          {
            case state_start:
              if (ch == '#')
                state = state_comment;
              else if (!std::isspace(ch))
              {
                config_entry.key = ch;
                state = state_cmd;
              }
              break;

            case state_cmd:
              if (ch == '\n')
              {
                config_values.push_back(config_entry);
                config_entry.clear();
                state = state_start;
              }
              else if (ch == '#')
              {
                config_values.push_back(config_entry);
                config_entry.clear();
                state = state_comment;
              }
              else if (std::isspace(ch))
                state = state_args;
              else
                config_entry.key += ch;
              break;

            case state_args:
              if (ch == '\n')
              {
                if (checkInclude(config_entry, istreams))
                  inptr = istreams.top();
                else
                  config_values.push_back(config_entry);
                config_entry.clear();
                state = state_start;
              }
              else if (ch == '#')
              {
                config_values.push_back(config_entry);
                config_entry.clear();
                state = state_comment;
              }
              else if (ch == '\\')
                state = state_args_esc;
              else if (ch == '"')
                state = state_qstring;
              else if (!std::isspace(ch))
              {
                token = ch;
                state = state_token;
              }
              break;

            case state_args_esc:
              if (ch == '\n')
                state = state_args;
              else
              {
                token = ch;
                state = state_token;
              }
              break;

            case state_token:
              if (ch == '\n')
              {
                config_entry.values.push_back(token);
                token.clear();
                if (checkInclude(config_entry, istreams))
                  inptr = istreams.top();
                else
                  config_values.push_back(config_entry);
                config_entry.clear();
                state = state_start;
              }
              else if (ch == '#')
              {
                config_entry.values.push_back(token);
                token.clear();
                config_values.push_back(config_entry);
                config_entry.clear();
                state = state_comment;
              }
              else if (std::isspace(ch))
              {
                config_entry.values.push_back(token);
                token.clear();
                state = state_args;
              }
              else
                token += ch;
              break;

            case state_qstring:
              if (ch == '"')
              {
                config_entry.values.push_back(token);
                token.clear();
                state = state_args;
              }
              else if (ch == '\\')
                state = state_qstring_esc;
              else
                token += ch;
              break;

            case state_qstring_esc:
              token += ch;
              state = state_qstring;
              break;

            case state_comment:
              if (ch == '\n')
                state = state_start;
              break;
          }
        }

        if (inptr == &in)
          inptr = 0;
        else
        {
          delete inptr;
          istreams.pop();
          inptr = istreams.top();
        }
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

  tntconfig::config_value_type tntconfig::getConfigValue(
       const std::string& key,
       const config_value_type& def) const
  {
    for (config_values_type::const_iterator it = config_values.begin();
         it != config_values.end(); ++it)
      if (it->key == key)
        return it->values;
    return def;
  }

  void tntconfig::getConfigValues(
       const std::string& key,
       config_values_type& ret) const
  {
    for (config_values_type::const_iterator it = config_values.begin();
         it != config_values.end(); ++it)
      if (it->key == key)
        ret.push_back(*it);
  }

  tntconfig::config_value_type::value_type tntconfig::getSingleValue(
       const std::string& key,
       const config_value_type::value_type& def) const
  {
    for (config_values_type::const_iterator it = config_values.begin();
         it != config_values.end(); ++it)
      if (it->key == key && it->values.size() > 0)
        return it->values[0];
    return def;
  }

}
