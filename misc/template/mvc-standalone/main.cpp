#include <iostream>
#include <fstream>

#include <tnt/tntnet.h>

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

    log_init(configuration.logConfiguration);

    tnt::Tntnet app;
    app.init(configuration);

    // Map static resources

    // Read static resources from file system when configured
    // This is useful for development, so that the application do not
    // need to be compiled and restarted on every change.
    if (!configuration.htdocs().empty())
    {
      app.mapUrl("^/(.*)", "static@tntnet")
         .setPathInfo(configuration.htdocs() + "/$1");
    }

    // Read static resources compiled into the binary
    app.mapUrl("^/(.*)", "resources")
       .setPathInfo("resources/$1");

    // ajax - map  /something.ext to component ext/something
    //        e.g. /request.json to json/request.ecpp
    //        e.g. /foo.html to html/foo.ecpp
    //
    // These are requests, which are not routed through webmain and hence
    // do not have the html frame.
    app.mapUrl("^/(.*)\\.(.*)$", "$2/$1");

    // Index page
    app.mapUrl("^/$", "controller/index");
    app.mapUrl("^/$", "webmain")
       .setArg("next", "view/index");

    // Controller
    app.mapUrl("^/(.*)$", "controller/$1");

    // View
    app.mapUrl("^/(.*)$", "webmain")
       .setArg("next", "view/$1");

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

