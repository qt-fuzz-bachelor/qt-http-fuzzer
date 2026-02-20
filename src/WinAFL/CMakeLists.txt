cmake_minimum_required(VERSION 4.1)
project(winAflFuzz LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set up directories
set(Qt6_DIR "C:/qtdev/build/qtbase/lib/cmake/Qt6")

# Qt setup
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer)

add_executable(winAflFuzz winAflFuzz.cpp)

# Link libraries
target_link_libraries(winAflFuzz
        PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
)
