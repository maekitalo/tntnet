#include <iostream>
#include <configuration.h>
#include <tnt/tntnet.h>
#include <tnt/configurator.h>
#include <cxxtools/log.h>

log_define("@PROJECT@")

int main(int argc, char* argv[])
{
  try
  {
    Configuration& configuration = Configuration::it();

    if (argc > 1)
      Configuration::readConfiguration(argv[1]);
    else
      Configuration::readConfiguration("@PROJECT@.conf");

    log_init(configuration.loggingConfiguration());

    tnt::Tntnet app;
    app.listen(configuration.listenIp(), configuration.listenPort());

    tnt::Configurator configurator(app);
    if (configuration.sessionTimeout())
      configurator.setSessionTimeout(configuration.sessionTimeout());

    // map static resources
    app.mapUrl("^/(.*)", "resources")
       .setPathInfo("resources/$1");

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

