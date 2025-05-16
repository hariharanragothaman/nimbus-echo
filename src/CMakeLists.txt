add_library(nimbus_core INTERFACE)
target_include_directories(nimbus_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nimbus_echo asio/echo_server.cpp)
target_link_libraries(nimbus_echo
        PRIVATE
        nimbus_core
        Boost::system        # only Boost lib actually required
)

add_executable(nimbus_udp_server asio/udp_echo_server.cpp)
add_executable(nimbus_udp_client asio/udp_echo_client.cpp)
target_link_libraries(nimbus_udp_server PRIVATE Boost::system)
target_link_libraries(nimbus_udp_client PRIVATE Boost::system)

# ── io_uring server (Linux only) ───────────────────────────────
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_library(URING_LIB uring REQUIRED)
    add_executable(nimbus_uring_echo uring/echo_server_uring.cpp)
    target_link_libraries(nimbus_uring_echo PRIVATE ${URING_LIB})
endif()


add_executable(nimbus_bench ../bench/roundtrip_bench.cpp)
target_link_libraries(nimbus_bench
        PRIVATE
        Boost::system
        benchmark::benchmark
        benchmark::benchmark_main   # pulls pthread etc.
)
