//
// Created by Hariharan Ragothaman on 5/15/25.
//

#include <benchmark/benchmark.h>
#include <boost/asio.hpp>
#include <thread>
#include <array>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

/* ---------- Helpers ------------------------------------------------------ */

// blocking Boost.Asio TCP echo client (loop‑back)
static void tcp_roundtrip(size_t payload) {
    boost::asio::io_context io;
    tcp::socket c(io);
    c.connect({boost::asio::ip::make_address("127.0.0.1"), 9000});
    std::string msg(payload, 'x');
    c.write_some(boost::asio::buffer(msg));
    std::array<char, 8192> buf{};
    c.read_some(boost::asio::buffer(buf, payload));
}

// blocking Boost.Asio UDP echo client
static void udp_roundtrip(size_t payload) {
    boost::asio::io_context io;
    udp::socket s(io, udp::v4());
    udp::endpoint server(boost::asio::ip::make_address("127.0.0.1"), 9001);
    std::string msg(payload, 'x');
    s.send_to(boost::asio::buffer(msg), server);
    std::array<char, 8192> buf{};
    s.receive_from(boost::asio::buffer(buf, payload), server);
}

#if defined(__linux__)
// tiny wrapper for the io_uring server (assumes it’s already running on 9002)
static void uring_tcp_roundtrip(size_t payload) {
    boost::asio::io_context io;
    tcp::socket c(io);
    c.connect({boost::asio::ip::make_address("127.0.0.1"), 9002});
    std::string msg(payload, 'x');
    c.write_some(boost::asio::buffer(msg));
    std::array<char, 8192> buf{};
    c.read_some(boost::asio::buffer(buf, payload));
}
#endif

/* ---------- Benchmarks --------------------------------------------------- */

static void BM_AsioTCP_64B(benchmark::State& s) {
    for (auto _ : s) tcp_roundtrip(64);
}
BENCHMARK(BM_AsioTCP_64B)->Unit(benchmark::kMicrosecond);

static void BM_AsioUDP_64B(benchmark::State& s) {
    for (auto _ : s) udp_roundtrip(64);
}
BENCHMARK(BM_AsioUDP_64B)->Unit(benchmark::kMicrosecond);

#if defined(__linux__)
static void BM_IouringTCP_64B(benchmark::State& s) {
    for (auto _ : s) uring_tcp_roundtrip(64);
}
BENCHMARK(BM_IouringTCP_64B)->Unit(benchmark::kMicrosecond);
#endif

BENCHMARK_MAIN();
