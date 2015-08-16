//          Copyright Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "http_server.h"

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

#include "log.h"

http_server::http_server(
        boost::asio::io_service& io_service,
        const std::string& host,
        const std::string& port) :
    io_service_(io_service),
    resolver_(io_service),
    acceptor_(io_service),
    query_(host, port)
{
}

void http_server::handle_resolve(
        const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        boost::asio::ip::tcp::resolver::iterator end;
        if (it != end)
        {
            boost::asio::ip::tcp::endpoint ep(*it);

            LOG_DEBUG() << "endpoint=["
                        << ep.address() << ":" << ep.port() << "]";

            acceptor_.open(ep.protocol());
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
            acceptor_.bind(ep);

            LOG_DEBUG() << "listening";
            acceptor_.listen();

            boost::system::error_code success;
            session::ptr null;
            handle_accept(success, null);
        }
    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }
}

void http_server::handle_accept(
        const boost::system::error_code& ec,
        session::ptr session_ptr)
{
    if (!ec)
    {
        if (session_ptr)
        {
            LOG_DEBUG() << "connection accepted";

            session_ptr->start();

            //sessions_.push_back(session_ptr);
        }

        session::ptr ptr = boost::make_shared<session>(
                    boost::ref(io_service_),
                    query_.host_name(),
                    query_.service_name());

        acceptor_.async_accept(
                    ptr->get_socket(),
                    boost::bind(
                        &http_server::handle_accept,
                        this,
                        boost::asio::placeholders::error,
                        ptr));

    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }
}

void http_server::start()
{
    LOG_DEBUG() << "starting host/service=[" << query_.host_name()
                << "/" << query_.service_name() << "]";

    resolver_.async_resolve(
                query_,
                boost::bind(
                    &http_server::handle_resolve,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator));
}

