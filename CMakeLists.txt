cmake_minimum_required(VERSION 3.22)
project(NimbusNet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Boost (header‑only Asio + libboost_system) ────────────────
find_package(Boost 1.78 REQUIRED COMPONENTS system)

# ── Catch2 (pulled automatically; tests only) ─────────────────
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
        GIT_TAG         v3.5.4         # same version as before
)
FetchContent_MakeAvailable(Catch2)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
