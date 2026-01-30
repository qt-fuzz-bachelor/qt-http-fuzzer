cmake_minimum_required(VERSION 3.25)
project(FuzzDemo LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer)

# Fuzzer + sanitizers
set(FUZZ_FLAGS
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
        -O1
        -g
)

add_executable(fuzz fuzz.cpp)

# Apply fuzz flags *to this target only*
target_compile_options(fuzz PRIVATE ${FUZZ_FLAGS})
target_link_options(fuzz PRIVATE ${FUZZ_FLAGS})

# Link Qt
target_link_libraries(fuzz
        Qt6::Core
        Qt6::Network
        Qt6::HttpServer
)

if(APPLE)
    # Homebrew LLVM (required for libFuzzer on macOS)
    set(HOME_BREW_LLVM "/opt/homebrew/opt/llvm@18")
    set(CMAKE_C_COMPILER   "${HOME_BREW_LLVM}/bin/clang")
    set(CMAKE_CXX_COMPILER "${HOME_BREW_LLVM}/bin/clang++")

    set_target_properties(fuzz PROPERTIES
            MACOSX_RPATH TRUE
            BUILD_RPATH "${HOME_BREW_LLVM}/lib"
    )
endif()
