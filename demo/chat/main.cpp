#include <iostream>
#include <tnt/tntnet.h>
#include <tnt/configurator.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("chat")

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<std::string> ip(argc, argv, 'l');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

    // configure logging by reading log.xml or log.properties
    log_init();

    // instantiate a application class and listen on a port
    tnt::Tntnet app;
    app.listen(ip, port);

    // map static resources
    app.mapUrl("^/(.*)", "resources")
       .setPathInfo("resources/$1");

    // map / to chat
    app.mapUrl("^/$", "chat");

    // map /comp to comp
    app.mapUrl("^/(.*)", "$1");

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

