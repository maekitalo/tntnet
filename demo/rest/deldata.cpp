/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <iostream>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'l');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

        cxxtools::http::Client client(ip, port);

        if (argc <= 1)
        {
            std::cerr << "missing parameter; usage: " << argv[0] << " key" << std::endl;
            return -1;
        }

        std::string key = argv[1];

        cxxtools::http::Request request('/' + key);
        request.method("DELETE");
        client.execute(request);
        const cxxtools::http::Reply& reply = client.readBody();

        std::cout << "http return code " << reply.httpReturnCode() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

