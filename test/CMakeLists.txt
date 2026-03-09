find_package(Qt6 REQUIRED COMPONENTS Test Network HttpServer)

set(CMAKE_AUTOMOC ON)

add_executable(test_server_runner
        test_server_core.cpp
        ../src/server_core.cpp
)

target_link_libraries(test_server_runner
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
        Qt6::Test
)

add_test(NAME test_server_runner COMMAND test_server_runner)
