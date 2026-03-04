# src/CMakeLists.txt
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer)

# Build the library
add_library(server_core
        server_core.cpp
        #parser_core.cpp
)

# Include directories for your headers and Qt headers
target_include_directories(server_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Network_INCLUDE_DIRS}
        ${Qt6HttpServer_INCLUDE_DIRS}
)

# Link Qt libraries
target_link_libraries(server_core
        PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
)
