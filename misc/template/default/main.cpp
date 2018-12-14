#include <tnt/tntnet.h>

#include <cxxtools/xml/xml.h>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    try
    {
        std::ifstream in("@PROJECT@.xml");
        tnt::TntConfig config;
        in >> cxxtools::xml::Xml(config);

        tnt::Tntnet app(config);

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

