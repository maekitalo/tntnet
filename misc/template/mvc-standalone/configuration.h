#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <cxxtools/serializationinfo.h>

class Configuration
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config);

  public:
    // return the static instance
    static Configuration& it();

    static void readConfiguration(const std::string& file);

    const std::string& listenIp() const
    { return _listenIp; }

    unsigned short listenPort() const
    { return _listenPort; }

    unsigned sessionTimeout() const
    { return _sessionTimeout; }

    const std::string& dburl() const
    { return _dburl; }

    const cxxtools::SerializationInfo& loggingConfiguration() const
    { return _loggingConfiguration; }

  private:
    Configuration();
    Configuration(const Configuration&);  // no implementation
    const Configuration& operator=(const Configuration&);  // no implementation

    std::string    _listenIp;
    unsigned short _listenPort;
    unsigned       _sessionTimeout;
    std::string    _dburl;
    cxxtools::SerializationInfo _loggingConfiguration;
};

#endif // CONFIGURATION_H
