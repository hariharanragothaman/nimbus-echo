add_executable(echo_test echo_test.cpp)

target_link_libraries(echo_test
        PRIVATE
        Boost::system
        Catch2::Catch2WithMain
)

add_test(NAME EchoRoundTrip COMMAND echo_test)

add_executable(udp_echo_test udp_echo_test.cpp)
target_link_libraries(udp_echo_test PRIVATE Boost::system Catch2::Catch2WithMain)
add_test(NAME UdpEchoRoundTrip COMMAND udp_echo_test)
