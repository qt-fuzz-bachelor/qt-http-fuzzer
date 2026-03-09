cmake_minimum_required(VERSION 3.16)

project(QHttpServerFuzzing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional but very common for fuzzing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(harness)
add_subdirectory(test)
