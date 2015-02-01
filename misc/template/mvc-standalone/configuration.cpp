#include <configuration.h>

void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config)
{
  si >>= static_cast<tnt::TntConfig&>(config);
  si.getMember("htdocs", config._htdocs);
  si.getMember("dburl", config._dburl);
  config._loggingConfiguration = si;
}

Configuration::Configuration()
{ }

Configuration& Configuration::it()
{
  static Configuration theConfiguration;
  return theConfiguration;
}
