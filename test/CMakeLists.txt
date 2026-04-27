find_package(Qt6 6.5 REQUIRED COMPONENTS
        Core
        Network
        Test
        HttpServer
        HttpServerPrivate
)

set(CMAKE_AUTOMOC ON)

function(add_qt_test name source)
    add_executable(${name} ${source})

    target_link_libraries(${name} PRIVATE
            Qt6::Core
            Qt6::Network
            Qt6::HttpServer
            Qt6::HttpServerPrivate
            Qt6::Test
            fuzzer_core
    )

    target_compile_definitions(${name} PRIVATE
            SRCDIR="${CMAKE_SOURCE_DIR}"
    )

    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_qt_test(test_server test_server_core.cpp)
add_qt_test(test_parser test_parser_core.cpp)
