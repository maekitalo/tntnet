#include <configuration.h>
#include <cxxtools/properties.h>
#include <fstream>

void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config)
{
  si.getMember("listen.ip", config._listenIp);  // listen.ip is optional - default is to listen on all local interfaces
  si.getMember("listen.port") >>= config._listenPort;
  si.getMember("sessiontimeout", config._sessionTimeout);  // sessiontimeout is optional
  si.getMember("htdocs", config._htdocs);
  si.getMember("dburl", config._dburl);
  config._loggingConfiguration = si;
}

Configuration::Configuration()
    : _listenPort(0),
      _sessionTimeout(0)
{ }

void Configuration::readConfiguration(const std::string& file)
{
  std::ifstream in(file.c_str());
  in >> cxxtools::Properties(Configuration::it());
}

Configuration& Configuration::it()
{
  static Configuration theConfiguration;
  return theConfiguration;
}
