//
// Created by Hariharan Ragothaman on 5/15/25.
//

// src/asio/echo_server.cpp
#include <boost/asio.hpp>
#include <array>
#include <iostream>

using boost::asio::ip::tcp;

class EchoSession : public std::enable_shared_from_this<EchoSession> {
    tcp::socket socket_;
    std::array<char, 4096> buf_{};

public:
    explicit EchoSession(tcp::socket s) : socket_(std::move(s)) {}
    void start() { read(); }

private:
    void read() {
        auto self = shared_from_this();
        socket_.async_read_some(boost::asio::buffer(buf_),
                                [this, self](auto ec, std::size_t n) { if (!ec) write(n); });
    }
    void write(std::size_t n) {
        auto self = shared_from_this();
        boost::asio::async_write(socket_, boost::asio::buffer(buf_, n),
                                 [this, self](auto ec, std::size_t) { if (!ec) read(); });
    }
};

int main() {
    boost::asio::io_context io;
    tcp::acceptor acc(io, {tcp::v4(), 9000});

    std::function<void()> do_accept = [&]() {
        acc.async_accept([&](auto ec, tcp::socket s) {
            if (!ec) std::make_shared<EchoSession>(std::move(s))->start();
            do_accept();
        });
    };
    do_accept();

    std::cout << "⚡  NimbusNet echo listening on 0.0.0.0:9000\n";
    io.run();
}
