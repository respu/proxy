//
//            Copyright (c) Marco Amorim 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
#include "session.h"

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

session::session(
        boost::asio::io_service& io_service,
        const std::string& host,
        const std::string& port,
        size_t buffer_size) :
    logger_(boost::log::keywords::channel = std::string("session") + std::string(".23")),
    io_service_(io_service),
    client_(io_service),
    server_(io_service),
    resolver_(io_service),
    to_(host, port),
    buffer_size_(buffer_size)
{
}

boost::asio::ip::tcp::socket& session::get_socket()
{
    return server_;
}

void session::start()
{
    LOG_DEBUG() << "session started";

    resolver_.async_resolve(
                to_,
                boost::bind(
                    &session::handle_resolve,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator)
                );
}

void session::handle_resolve(
        const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator it)
{
    if (!ec)
    {
        boost::asio::ip::tcp::resolver::iterator end;
        if (it != end)
        {
            boost::asio::ip::tcp::endpoint ep(*it);

            LOG_DEBUG() << "to endpoint=["
                        << ep.address() << ":" << ep.port() << "]";

            client_.async_connect(
                        ep,
                        boost::bind(
                            &session::handle_connect,
                            this,
                            boost::asio::placeholders::error));
        }
    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }

}

void session::handle_connect(
        const boost::system::error_code& ec)
{
    if (!ec)
    {
        LOG_DEBUG() << "connected";

        sp_buffer buffer = std::make_pair(
                    boost::make_shared<uint8_t[]>(buffer_size_), buffer_size_);

        client_.async_read_some(
                    boost::asio::buffer(
                        buffer.first.get(), buffer.second),
                    boost::bind(
                        &session::handle_read, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        buffer,
                        boost::ref(client_),
                        boost::ref(server_)));

        buffer = std::make_pair(
                    boost::make_shared<uint8_t[]>(buffer_size_), buffer_size_);

        server_.async_read_some(
                    boost::asio::buffer(
                        buffer.first.get(), buffer.second),
                    boost::bind(
                        &session::handle_read, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        buffer,
                        boost::ref(server_),
                        boost::ref(client_)));
    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }
}

void session::handle_read(
        const boost::system::error_code& ec,
        size_t bytes_tranferred,
        sp_buffer buffer_read,
        boost::asio::ip::tcp::socket& from,
        boost::asio::ip::tcp::socket& to)
{
    LOG_TRACE() << "bytes received: " << bytes_tranferred;

    if (!ec && bytes_tranferred)
    {
        to.async_send(
                    boost::asio::buffer(
                        buffer_read.first.get(),
                        bytes_tranferred),
                    boost::bind(
                        &session::handle_send,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        buffer_read));

        sp_buffer buffer = std::make_pair(
                    boost::make_shared<uint8_t[]>(buffer_size_), buffer_size_);

        from.async_read_some(
                    boost::asio::buffer(
                        buffer.first.get(), buffer.second),
                    boost::bind(
                        &session::handle_read, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        buffer,
                        boost::ref(from),
                        boost::ref(to)));

    }
    else
    {
        if (!ec)
        {
            LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
        }
        else
        {
            LOG_DEBUG() << "connection closed";
        }

        server_.close();
    }

}

void session::handle_send(
        const boost::system::error_code& ec,
        size_t bytes_tranferred,
        sp_buffer)
{
    LOG_TRACE() << "bytes sent: " << bytes_tranferred;

    if (!ec)
    {

    }
    else
    {
        LOG_ERROR() << "ec=[" << ec << "] message=[" << ec.message() << "]";
    }
}

