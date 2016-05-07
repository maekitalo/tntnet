/*
 * Copyright (C) 2016 Tommi Maekitalo
 *
 */

#include <iostream>
#include <tnt/http.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>
#include <cxxtools/http/client.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'l');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 8000);

        cxxtools::http::Client client(ip, port);

        for (int a = 1; a < argc; ++a)
        {
            const cxxtools::http::Reply& reply = client.get(std::string("/") + argv[a]);
            if (reply.httpReturnCode() == HTTP_OK)
                std::cout << reply.body() << std::endl;
            else
                std::cout << "http return code " << reply.httpReturnCode() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

