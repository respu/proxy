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
            const std::string& shost,
            const std::string& sport,
            const std::string& dhost,
            const std::string& dport,
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

    void handle_signal(
            const boost::system::error_code& error,
            int signal_number);

    boost::asio::io_service& io_service_;

    boost::asio::ip::tcp::socket client_;

    boost::asio::ip::tcp::socket server_;

    boost::asio::ip::tcp::acceptor acceptor_;

    boost::asio::ip::tcp::resolver resolver_;

    boost::asio::ip::tcp::resolver::query from_;

    boost::asio::ip::tcp::resolver::query to_;

    boost::asio::signal_set signal_set_;

    std::vector<session::ptr> sessions_;

    size_t buffer_size_;

};
