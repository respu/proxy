//          Copyright Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "session.h"

class http_server :
        public boost::enable_shared_from_this<http_server>
{

public:

    http_server(
            boost::asio::io_service& io_service,
            const std::string& host,
            const std::string& port);

    void start();

protected:

    void handle_resolve(
            const boost::system::error_code& ec,
            boost::asio::ip::tcp::resolver::iterator it);

    void handle_accept(
            const boost::system::error_code& ec,
            session::ptr session);

    boost::asio::io_service& io_service_;

    boost::asio::ip::tcp::resolver resolver_;

    boost::asio::ip::tcp::acceptor acceptor_;

    boost::asio::ip::tcp::resolver::query query_;

};
