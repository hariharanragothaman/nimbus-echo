//
// Created by Hariharan Ragothaman on 5/15/25.
//

#include <catch2/catch_test_macros.hpp>
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::udp;

TEST_CASE("UDP echo round‑trip") {
boost::asio::io_context io;
udp::socket svr(io, {udp::v4(), 9800});

std::thread server([&]{
    std::array<char, 16> buf{};
    udp::endpoint cli;
    auto n = svr.receive_from(boost::asio::buffer(buf), cli);
    svr.send_to(boost::asio::buffer(buf, n), cli);
});

udp::socket cli(io, udp::v4());
udp::endpoint server_ep(boost::asio::ip::make_address("127.0.0.1"), 9800);
std::string msg = "pong";
cli.send_to(boost::asio::buffer(msg), server_ep);
char buf[16];
std::size_t n = cli.receive_from(boost::asio::buffer(buf), server_ep);
REQUIRE(std::string(buf, n) == msg);

server.join();
}
