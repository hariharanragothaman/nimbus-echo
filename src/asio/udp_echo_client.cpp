#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <numeric>          // <-- for std::accumulate
#include <vector>

using boost::asio::ip::udp;
using SteadyClock = std::chrono::steady_clock;   // renamed alias

int main(int argc, char* argv[]) {
    std::size_t count = (argc > 1) ? std::stoul(argv[1]) : 100000;

    boost::asio::io_context io;
    udp::socket sock(io, udp::v4());
    udp::endpoint server(boost::asio::ip::make_address("127.0.0.1"), 9001);

    std::array<char, 8> ping{{'p','i','n','g'}};
    std::array<char, 8> pong{};
    std::vector<double> lat_ns;
    lat_ns.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        auto t0 = SteadyClock::now();
        sock.send_to(boost::asio::buffer(ping), server);
        sock.receive_from(boost::asio::buffer(pong), server);
        auto dt = std::chrono::duration<double, std::nano>(SteadyClock::now() - t0).count();
        lat_ns.push_back(dt);
    }

    double avg = std::accumulate(lat_ns.begin(), lat_ns.end(), 0.0) / lat_ns.size();
    std::cout << "avg‑RTT: " << avg / 1000.0 << " µs (" << count << " packets)\n";
}
