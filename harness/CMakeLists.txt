# List of fuzz modes and their macros
option(FUZZ_HTTP_SERVER "Enable HTTP server fuzzing" OFF)
option(FUZZ_HTTP_PARSER "Enable HTTP parser fuzzing" OFF)
option(FUZZ_HEADER_PARSER "Enable HTTP header parser fuzzing" OFF)
option(USE_ASAN "Enable AddressSanitizer (ASAN)" OFF)

# Determine which fuzz mode was selected
if(FUZZ_HTTP_SERVER)
    set(FUZZ_MACRO "FUZZ_HTTP_SERVER")
    set(FUZZ_NAME "server")
elseif(FUZZ_HTTP_PARSER)
    set(FUZZ_MACRO "FUZZ_HTTP_PARSER")
    set(FUZZ_NAME "parser")
elseif(FUZZ_HEADER_PARSER)
    set(FUZZ_MACRO "FUZZ_HEADER_PARSER")
    set(FUZZ_NAME "header")
else()
    message(FATAL_ERROR "No fuzzing mode defined! Pass -DFUZZ_HTTP_SERVER=ON, -DFUZZ_HTTP_PARSER=ON, or -DFUZZ_HEADER_PARSER=ON")
endif()

# Build target name
set(FUZZ_TARGET "fuzz_${FUZZ_NAME}_${FUZZER_ENGINE}")

# Create the executable from the shared source
add_executable(${FUZZ_TARGET} fuzz_${FUZZER_ENGINE}.cpp)

# Define the selected macro for compilation
target_compile_definitions(${FUZZ_TARGET} PRIVATE ${FUZZ_MACRO})

# Link libraries
target_link_libraries(${FUZZ_TARGET} PRIVATE fuzzer_core)
# Engine-specific flags
if(FUZZER_ENGINE STREQUAL "afl")
    set(FUZZ_FLAGS -fno-omit-frame-pointer -O1 -g)
    if(USE_ASAN)
        list(APPEND FUZZ_FLAGS -fsanitize=address)
    endif()
elseif(FUZZER_ENGINE STREQUAL "libfuzzer")
    set(FUZZ_FLAGS -fno-omit-frame-pointer -O1 -g -fsanitize=fuzzer)
    if(USE_ASAN)
        list(POP_BACK FUZZ_FLAGS)
        list(APPEND FUZZ_FLAGS -fsanitize=fuzzer,address)
    endif()
endif()

# Apply flags
if(UNIX AND FUZZ_FLAGS)
    target_compile_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
    target_link_options(${FUZZ_TARGET} PRIVATE ${FUZZ_FLAGS})
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
