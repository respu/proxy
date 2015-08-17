//          Copyright Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "proxy.h"

int main(int argc, char** argv)
{
    try
    {
        namespace po = boost::program_options;
        po::variables_map vm;
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "this help message")
                ("saddr", po::value<std::string>()->default_value("localhost"), "source address")
                ("sport", po::value<std::string>()->default_value("http-alt"), "source service name or port")
                ("daddr", po::value<std::string>()->default_value("localhost"), "destination address")
                ("dport", po::value<std::string>()->default_value("http"), "destination service name or port");

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        boost::asio::io_service io_service;

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        proxy service(
                    io_service,
                    vm["saddr"].as<std::string>(),
                    vm["sport"].as<std::string>(),
                    vm["daddr"].as<std::string>(),
                    vm["dport"].as<std::string>());

        service.start();
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
    }

    return 0;
}
