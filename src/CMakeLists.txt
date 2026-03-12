# src/CMakeLists.txt
find_package(Qt6 6.5 REQUIRED COMPONENTS Core HttpServer HttpServerPrivate)

# Build the library
add_library(fuzzer_core
        server_core.cpp
        parser_core.cpp
)

# Include directories for your headers and Qt headers
target_include_directories(fuzzer_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Network_INCLUDE_DIRS}
        ${Qt6HttpServer_INCLUDE_DIRS}
)

# Link Qt libraries
target_link_libraries(fuzzer_core
        PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::HttpServerPrivate
)
