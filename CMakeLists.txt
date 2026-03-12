cmake_minimum_required(VERSION 3.16)

project(QHttpServerFuzzing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

# ---- Fuzzing engine selection ----
set(FUZZER_ENGINE "" CACHE STRING "Choose fuzzing engine")

if(WIN32)
    if(NOT FUZZER_ENGINE)
        set(FUZZER_ENGINE "winafl" CACHE STRING "" FORCE)
    endif()
    set(valid_fuzzers winafl)
else()
    set(valid_fuzzers libfuzzer afl)
endif()

set_property(CACHE FUZZER_ENGINE PROPERTY STRINGS ${valid_fuzzers})

if(NOT WIN32 AND NOT FUZZER_ENGINE)
    message(FATAL_ERROR
            "Please select a fuzzing engine:
        -DFUZZER_ENGINE=libfuzzer
        -DFUZZER_ENGINE=afl")
endif()

if(NOT FUZZER_ENGINE IN_LIST valid_fuzzers)
    message(FATAL_ERROR "Invalid FUZZER_ENGINE: ${FUZZER_ENGINE}")
endif()

message(STATUS "Using fuzzing engine: ${FUZZER_ENGINE}")

# ---- Subprojects ----
add_subdirectory(src)
add_subdirectory(harness)
add_subdirectory(test)
