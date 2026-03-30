find_package(Qt6 REQUIRED COMPONENTS Test Network HttpServer)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core HttpServer HttpServerPrivate)

set(CMAKE_AUTOMOC ON)

target_link_libraries(${test_server_runner}  fuzzer_core)

add_executable(test_server_runner
        test_server_core.cpp
)

target_link_libraries(test_server_runner
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
        Qt6::HttpServerPrivate
        Qt6::Test
        fuzzer_core
)

add_test(NAME test_server_runner COMMAND test_server_runner)

add_executable(test_parser_core
        test_parser_core.cpp
)

target_link_libraries(test_parser_core PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
        Qt6::HttpServerPrivate
        Qt6::Test
        fuzzer_core
)

add_test(NAME test_parser_core COMMAND test_parser_core)
