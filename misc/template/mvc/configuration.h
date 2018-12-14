#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <tnt/tntconfig.h>

class Configuration : public tnt::TntConfig
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, Configuration& config);

  public:
    // return the static instance
    static Configuration& it();

    const std::string& htdocs() const
    { return _htdocs; }

    const std::string& dburl() const
    { return _dburl; }

  private:
    Configuration();
    Configuration(const Configuration&);  // no implementation
    const Configuration& operator=(const Configuration&);  // no implementation

    std::string    _htdocs;
    std::string    _dburl;
};

#endif // CONFIGURATION_H
