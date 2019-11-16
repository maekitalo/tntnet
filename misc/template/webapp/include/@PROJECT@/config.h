#ifndef @PROJECT@_CONFIG_H
#define @PROJECT@_CONFIG_H

#include <tnt/tntconfig.h>

namespace @PROJECT@
{

// This class defines specifies the configuraton variables needed in the web
// applicatoin. The base class is `tnt::TntConfig` so that all configurations
// defined in tntnet.xml can be used here.
// By deriving from that class you may add own variables there. As examples 2
// variables are defined: _htdocs_ and _dburl_. _htdocs_ is used by the
// application to load static files from the file system. _dburl_ specifies the
// database used in the db examples. It uses `tntdb` to access a database and
// hence the rules for dburls of tntdb are used here.

// To add own variables, add member variables here and add them to the
// deserialization operator in `src/@PROJECT@/config.cpp`.

class Config : public tnt::TntConfig
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, Config& config);

    std::string _htdocs;
    std::string _dburl;

public:
    Config();
    const std::string& htdocs() const     { return _htdocs; }
    const std::string& dburl() const      { return _dburl; }

    static const Config& it();
};

}

#endif
