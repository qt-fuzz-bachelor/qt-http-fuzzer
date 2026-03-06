# harnesses/parser_only/CMakeLists.txt
set(FUZZ_TARGET "fuzz_server_${FUZZER_ENGINE}")

add_executable(${FUZZ_TARGET}
        ${FUZZ_TARGET}.cpp
)

target_link_libraries(${FUZZ_TARGET} PRIVATE fuzzer_core)

# Common flags for engines that support them
set(COMMON_FLAGS
        -fno-omit-frame-pointer
        -O1
        -g
)

# Engine-specific flags
set(FUZZ_FLAGS "")
if(FUZZER_ENGINE STREQUAL "afl")
    if(NOT WIN32)  # AFL flags only apply on non-Windows
        set(FUZZ_FLAGS "${COMMON_FLAGS} \ -fsanitize=address")
    endif()
elseif(FUZZER_ENGINE STREQUAL "libfuzzer")
    set(FUZZ_FLAGS "${COMMON_FLAGS} \ -fsanitize=fuzzer,address,undefined")
endif()

# Platform-specific tweaks
if(APPLE AND FUZZER_ENGINE STREQUAL "afl")
    set(AFL_NO_FORKSRV 1)
    set(MallocNanoZone 0)
    set_target_properties(${FUZZ_TARGET} PROPERTIES
            MACOSX_RPATH TRUE
            BUILD_RPATH "@executable_path/../Frameworks"
    )
endif()

# Apply flags only if they were defined
if(UNIX AND FUZZ_FLAGS)
    target_compile_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
    target_link_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
endif()
