//
// Created by Hariharan Ragothaman on 5/15/25.
//

#include <boost/asio.hpp>
#include <array>
#include <iostream>

using boost::asio::ip::udp;

class UdpEchoServer {
    udp::socket socket_;
    std::array<char, 4096> buf_{};
    udp::endpoint remote_;
public:
    explicit UdpEchoServer(boost::asio::io_context& io, unsigned short port)
            : socket_(io, udp::endpoint{udp::v4(), port}) { receive(); }

private:
    void receive() {
        socket_.async_receive_from(
                boost::asio::buffer(buf_), remote_,
                [this](auto ec, std::size_t n) {
                    if (!ec) send(n);
                });
    }
    void send(std::size_t n) {
        socket_.async_send_to(
                boost::asio::buffer(buf_, n), remote_,
                [this](auto /*ec*/, std::size_t /*n*/) { receive(); });
    }
};

int main() {
    try {
        boost::asio::io_context io;
        UdpEchoServer srv(io, 9001);
        std::cout << "⚡  UDP echo on 0.0.0.0:9001\n";
        io.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
