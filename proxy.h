//          Copyright Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr.hpp>

#include "session.h"

class proxy :
        public boost::enable_shared_from_this<proxy>
{

public:

    typedef boost::shared_ptr<proxy> ptr;

    typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

    proxy(
            boost::asio::io_service& io_service,
            const std::string& from_host,
            const std::string& from_port,
            const std::string& to_host,
            const std::string& to_port,
            size_t buffer_size = 4096);

    virtual ~proxy();

    void start();

protected:

    void handle_resolve(
            const boost::system::error_code& ec,
            boost::asio::ip::tcp::resolver::iterator it);

    void handle_accept(
            const boost::system::error_code& ec,
            session::ptr session);

    boost::asio::io_service& io_service_;

    boost::asio::ip::tcp::socket client_;

    boost::asio::ip::tcp::socket server_;

    boost::asio::ip::tcp::acceptor acceptor_;

    boost::asio::ip::tcp::resolver resolver_;

    boost::asio::ip::tcp::resolver::query from_;

    boost::asio::ip::tcp::resolver::query to_;

    std::vector<session::ptr> sessions_;

    size_t buffer_size_;

};
