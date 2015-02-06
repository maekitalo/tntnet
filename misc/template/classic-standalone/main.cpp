#include <iostream>
#include <tnt/tntnet.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

log_define("@PROJECT@")

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<std::string> ip(argc, argv, 'l');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

    log_init();

    tnt::Tntnet app;
    app.listen(ip, port);

    // map static resources
    app.mapUrl("^/(.*)", "resources")
       .setPathInfo("resources/$1");

    // map / to @PROJECT@
    app.mapUrl("^/$", "@PROJECT@");

    // map /comp to comp
    app.mapUrl("^/(.*)", "$1");

    app.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

