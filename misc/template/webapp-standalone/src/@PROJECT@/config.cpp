#include <@PROJECT@/config.h>
#include <cxxtools/envsubst.h>

namespace @PROJECT@
{

static const Config* theConfiguration;

Config::Config()
{
    theConfiguration = this;
}

const Config& Config::it()
{
    return *theConfiguration;
}


void operator>>= (const cxxtools::SerializationInfo& si, Config& config)
{
    si >>= static_cast<tnt::TntConfig&>(config);
    si.getMember("htdocs", config._htdocs);
    si.getMember("dburl", config._dburl);

    config._htdocs = cxxtools::envSubst(config._htdocs);
}

}
