#include <@PROJECT@/config.h>
#include <tnt/tntnet.h>
#include <cxxtools/xml/xml.h>
#include <cxxtools/log.h>
#include <iostream>
#include <fstream>

log_define("@PROJECT@")

int main(int argc, char* argv[])
{
    try
    {
        std::ifstream cfgfile("@PROJECT@.xml");
        @PROJECT@::Config config;
        cfgfile >> cxxtools::xml::Xml(config);

        tnt::Tntnet app(config);

        if (!config.htdocs().empty())
        {
            app.mapUrl("^/(.*)", "static@tntnet")
               .setPathInfo(config.htdocs() + "/$1");
        }

        // map static resources
        app.mapUrl("^/(.*)", "resources")
             .setPathInfo("resources/$1");

        // map / to webmain
        app.mapUrl("^/$", "webmain");

        // Actions
        //
        app.mapUrl("^/(.*)\\.action$", "actionmain")
           .setArg("next", "$1");

        // map  /something.ext to component ext/something
        //        e.g. /request.json to json/request.ecpp
        //        e.g. /foo.html to html/foo.ecpp
        //
        // These are requests, which are not routed through webmain and hence
        // do not have the html frame.
        app.mapUrl("^/(.*)\\.(.*)$", "$2/$1");

        // map /comp to comp
        app.mapUrl("^/(.*)", "$1");

        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
