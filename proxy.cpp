//          Copyright Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include "log.h"
#include "proxy.h"

using namespace boost::asio;

proxy::proxy(
        boost::asio::io_service& io_service,
        const std::string& from_host,
        const std::string& from_port,
        const std::string& to_host,
        const std::string& to_port,
        size_t buffer_size) :
    io_service_(io_service),
    client_(io_service),
    server_(io_service),
    acceptor_(io_service),
    resolver_(io_service),
    from_(from_host, from_port),
    to_(to_host, to_port),
    buffer_size_(buffer_size)
{
}

proxy::~proxy()
{
}

void proxy::start()
{
    LOG_DEBUG() << "starting proxy "
                << "from=[" << from_.host_name()
                << ":" << from_.service_name() << "] "
                << "to=[" << to_.host_name()
                << ":" << to_.service_name() << "]";

    resolver_.async_resolve(
                from_,
                boost::bind(
                    &proxy::handle_resolve,
                    this,
                    placeholders::error,
                    placeholders::iterator)
                );
}

void proxy::handle_resolve(
        const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        ip::tcp::resolver::iterator end;
        if (it != end)
        {
            ip::tcp::endpoint ep(*it);

            LOG_DEBUG() << "from endpoint=["
                        << ep.address() << ":" << ep.port() << "]";

            acceptor_.open(ep.protocol());
            acceptor_.set_option(socket_base::reuse_address(true));
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

void proxy::handle_accept(
        const boost::system::error_code& ec,
        session::ptr session_ptr)
{
    if (!ec)
    {
        if (session_ptr)
        {
            LOG_DEBUG() << "connection accepted";

            session_ptr->start();

            sessions_.push_back(session_ptr);
        }

        session::ptr ptr = boost::make_shared<session>(
                    boost::ref(io_service_),
                    to_.host_name(),
                    to_.service_name(),
                    buffer_size_);

        acceptor_.async_accept(
                    ptr->get_socket(),
                    boost::bind(
                        &proxy::handle_accept,
                        this,
                        placeholders::error,
                        ptr));

    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }
}
