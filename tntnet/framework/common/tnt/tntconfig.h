////////////////////////////////////////////////////////////////////////
// tntconfig.h
//

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

