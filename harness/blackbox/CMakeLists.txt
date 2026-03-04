# Define selectable cache variable (initially empty)
set(FUZZER_ENGINE "" CACHE STRING
        "Choose fuzzing engine")

# Platform-specific allowed values
if(WIN32)
    # Auto-select winafl if user did not specify anything
    if(NOT FUZZER_ENGINE)
        set(FUZZER_ENGINE "winafl" CACHE STRING "" FORCE)
    endif()

    set(valid_fuzzers winafl)

else() # Unix-like (Linux, macOS, etc.)
    set(valid_fuzzers libfuzzer afl)
endif()

# Show dropdown values in cmake-gui
set_property(CACHE FUZZER_ENGINE PROPERTY STRINGS ${valid_fuzzers})

# On Unix, require explicit selection
if(NOT WIN32 AND NOT FUZZER_ENGINE)
    message(FATAL_ERROR
            "Please select a fuzzing engine:
         -DFUZZER_ENGINE=libfuzzer
         -DFUZZER_ENGINE=afl")
endif()

# Validate value
if(NOT FUZZER_ENGINE IN_LIST valid_fuzzers)
    message(FATAL_ERROR
            "Invalid FUZZER_ENGINE: ${FUZZER_ENGINE}")
endif()

message(STATUS "Using fuzzing engine: ${FUZZER_ENGINE}")

add_executable(fuzz_server_${FUZZER_ENGINE}
        fuzz_server_${FUZZER_ENGINE}.cpp
)

target_link_libraries(fuzz_server_${FUZZER_ENGINE}
        PRIVATE
        server_core
)

if(FUZZER_ENGINE STREQUAL "afl")
    if(APPLE)
        set(AFL_NO_FORKSRV 1)
        set(MallocNanoZone 0)

        set_target_properties(fuzz_server_${FUZZER_ENGINE} PROPERTIES
                MACOSX_RPATH TRUE
                BUILD_RPATH "@executable_path/../Frameworks"
        )
    endif()

    set(FUZZ_FLAGS
            -fsanitize=address
            -fno-omit-frame-pointer
            -O1
            -g
    )

    target_compile_options(fuzz_server_${FUZZER_ENGINE} PRIVATE ${FUZZ_FLAGS})
    target_link_options(fuzz_server_${FUZZER_ENGINE} PRIVATE ${FUZZ_FLAGS})
elseif(FUZZER_ENGINE STREQUAL "libfuzzer")
    set(FUZZ_FLAGS
            -fsanitize=fuzzer,address,undefined
            -fno-omit-frame-pointer
            -O1
            -g
    )

    target_compile_options(fuzz_server_${FUZZER_ENGINE} PRIVATE ${FUZZ_FLAGS})
    target_link_options(fuzz_server_${FUZZER_ENGINE} PRIVATE ${FUZZ_FLAGS})
endif()
