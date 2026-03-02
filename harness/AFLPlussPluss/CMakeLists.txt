cmake_minimum_required(VERSION 3.25)
project(TestAFL++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use AFL++ compilers
set(CMAKE_C_COMPILER   "/opt/homebrew/bin/afl-clang-fast")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/afl-clang-fast++")

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

# Fuzzer + sanitizers
set(AFL_FLAGS
        -fsanitize=address
        -fno-omit-frame-pointer
        -O1
        -g
)


# Fuzzer target
add_executable(fuzz_qt
        fuzzAFL.cpp
)

target_compile_options(fuzz_qt PRIVATE ${AFL_FLAGS})
target_link_options(fuzz_qt PRIVATE ${AFL_FLAGS})

target_link_libraries(fuzz_qt PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
)


#Only Mac

if(APPLE)
    set_target_properties(fuzz_qt PROPERTIES
            MACOSX_RPATH TRUE
            BUILD_RPATH "@executable_path/../Frameworks"
    )
endif()
