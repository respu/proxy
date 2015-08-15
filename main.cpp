#include <iostream>

#include "proxy.h"
#include "http_server.h"

int main()
{
    try
    {
        boost::asio::io_service io_service;
        //http_server server(io_service, "0.0.0.0", "http-alt");
        //server.start();

        proxy instance(io_service, "localhost", "2222", "localhost", "22");
        instance.start();
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
    }

    return 0;
}
