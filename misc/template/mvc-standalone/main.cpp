#include <iostream>
#include <fstream>

#include <tnt/tntnet.h>
#include <tnt/configurator.h>

#include <cxxtools/xml/xml.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include "configuration.h"

log_define("@PROJECT@")

int main(int argc, char* argv[])
{
  try
  {
    Configuration& configuration = Configuration::it();

    // read option "-c <configfile>" with defautl value "@PROJECT@.xml":
    cxxtools::Arg<const char*> configfilename(argc, argv, 'c', "@PROJECT@.xml");
    std::ifstream configfile(configfilename);
    if (!configfile)
      throw std::runtime_error(std::string("could not open configuration file ") + configfilename.getValue());

    configfile >> cxxtools::xml::Xml(configuration);

    log_init(configuration.loggingConfiguration());

    tnt::Tntnet app;
    app.init(configuration);

    // map static resources

    // read static resources from file system when configured
    if (!configuration.htdocs().empty())
    {
      app.mapUrl("^/(.*)", "static@tntnet")
         .setPathInfo(configuration.htdocs() + "/$1");
    }

    // read static resources compiled into the binary
    app.mapUrl("^/(.*)", "resources")
       .setPathInfo("resources/$1");

    // ajax
    app.mapUrl("^/(.*).json$", "json/$1");  // json requests
    app.mapUrl("^/(.*).html$", "html/$1");  // html fragments

    // index page
    app.mapUrl("^/$", "controller/index");
    app.mapUrl("^/$", "webmain")
       .setArg("next", "view/index");

    // controller
    app.mapUrl("^/(.*)$", "controller/$1");

    // view
    app.mapUrl("^/(.*)$", "webmain")
       .setArg("next", "view/$1");

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

