// tests/echo_test.cpp
#include <catch2/catch_test_macros.hpp>   // v3 header – no main needed
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

TEST_CASE("Echo round‑trip") {
boost::asio::io_context io;
tcp::acceptor acc(io, {tcp::v4(), 9900});

std::thread server([&]{
    tcp::socket s = acc.accept();
    char buf[16];
    std::size_t n = s.read_some(boost::asio::buffer(buf));
    boost::asio::write(s, boost::asio::buffer(buf, n));
});

tcp::socket c(io);
c.connect({boost::asio::ip::make_address("127.0.0.1"), 9900});
std::string msg = "ping";
boost::asio::write(c, boost::asio::buffer(msg));

char reply[16];
std::size_t n = c.read_some(boost::asio::buffer(reply));
REQUIRE(std::string(reply, n) == msg);

server.join();
}
