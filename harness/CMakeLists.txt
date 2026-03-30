# List of fuzz modes and their macros
set(FUZZ_MODES
        "server;FUZZ_HTTP_SERVER"
        "parser;FUZZ_HTTP_PARSER"
        "header;FUZZ_HEADER_PARSER"
)

foreach(mode_pair IN LISTS FUZZ_MODES)
    # Split mode_pair into target suffix and macro
    string(REPLACE ";" ";" mode_split ${mode_pair})
    list(GET mode_split 0 mode_name)
    list(GET mode_split 1 mode_macro)

    # Build target name
    set(FUZZ_TARGET "fuzz_${mode_name}_${FUZZER_ENGINE}")

    # Create executable from the common source file
    add_executable(${FUZZ_TARGET} fuzz.cpp)

    # Set the mode-specific macro
    target_compile_definitions(${FUZZ_TARGET} PRIVATE ${mode_macro})

    # Link core fuzzing library
    target_link_libraries(${FUZZ_TARGET} PRIVATE fuzzer_core)

    # Engine-specific flags
    option(USE_ASAN "Enable AddressSanitizer" OFF)
    if(FUZZER_ENGINE STREQUAL "afl")
        if(NOT WIN32)
            set(FUZZ_FLAGS -fno-omit-frame-pointer -O1 -g)
            if(USE_ASAN)
                list(APPEND FUZZ_FLAGS -fsanitize=address)
            endif()
        endif()
    elseif(FUZZER_ENGINE STREQUAL "libfuzzer")
        set(FUZZ_FLAGS -fno-omit-frame-pointer -O1 -g -fsanitize=fuzzer)
        if(USE_ASAN)
            list(POP_BACK FUZZ_FLAGS)
            list(APPEND FUZZ_FLAGS -fsanitize=fuzzer,address)
        endif()
    endif()

    # Platform-specific tweaks (macOS)
    if(APPLE AND FUZZER_ENGINE STREQUAL "afl")
        set(AFL_NO_FORKSRV 1)
        set(MallocNanoZone 0)
        set_target_properties(${FUZZ_TARGET} PROPERTIES
                MACOSX_RPATH TRUE
                BUILD_RPATH "@executable_path/../Frameworks"
        )
    endif()

    # Apply flags if they exist
    if(UNIX AND FUZZ_FLAGS)
        target_compile_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
        target_link_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
    endif()
endforeach()
