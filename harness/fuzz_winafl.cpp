// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file fuzz_parser_winafl.cpp
 * @brief WinAFL fuzzing harness for isolated fuzzing of the HTTP parser.
 *
 * This translation unit provides a file-based fuzzing harness compatible with
 * Windows AFL or other custom fuzzing frameworks. It reads input from a file
 * and forwards it to the isolated parser fuzzing interface to stress-test
 * parsing logic.
 *
 * Optional looping mode allows repeated testing of the same input.
 */

#include "parser_core.h"  // NOLINT(build/include_subdir)
#include "server_core.h"  // NOLINT(build/include_subdir)
#include <iostream>       // NOLINT(build/include_order)
#include <windows.h>      // NOLINT(build/include_order)

/**
 * @brief File-based fuzzing entry point.
 *
 * This function reads the fuzzed payload from a file and injects it into
 * the selected fuzzing interface.
 *
 * @param f_path Path to the input file containing fuzzed data.
 *
 * @return Always returns 0.
 *
 * @note The function is marked `__declspec(dllexport)` and
 *       `__declspec(noinline)` to facilitate instrumentation by fuzzing tools.
 */
extern "C" __declspec(dllexport) __declspec(noinline) int target(char *f_path) {
  // Inject the fuzzed payload to the HTTP server
#ifdef FUZZ_HTTP_SERVER
  fuzzServerBlackbox(f_path);
#elif defined(FUZZ_HTTP_PARSER)
  fuzzHttpParserOnly(f_path);
#elif defined(FUZZ_HEADER_PARSER)
  fuzzHeaderParserOnly(f_path);
#else
#error "No fuzzing mode defined! Please define one of " \
"FUZZ_HTTP_SERVER, FUZZ_HTTP_PARSER, FUZZ_HEADER_PARSER"
#endif
  return 0;
}

/**
 * @brief Standalone executable entry point.
 *
 * This program can be invoked from the command line with a fuzz input file.
 * Optionally, the "loop" argument can be passed to repeatedly feed the same
 * input to the fuzzing function for continuous stress testing.
 *
 * Usage:
 * @code
 * ./fuzz_server_winafl <input_file> [loop]
 * @endcode
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments:
 *             - argv[1]: path to the input file
 *             - argv[2] (optional): "loop" to repeat indefinitely
 *
 * @return Returns 0 on completion.
 */
int main(int argc, char **argv) {
  // Check if at least one argument (input file) is provided
  if (argc < 2) {
    std::cout << "Usage:" << argv[0] << "<input file> [loop]";
    return 0;
  }

  // Optional "loop" mode: repeatedly fuzz the same input indefinitely
  if (argc == 3 && !strcmp(argv[2], "loop")) {
    while (true) {
      target(argv[1]);
    }
  }

  return target(argv[1]);
}
