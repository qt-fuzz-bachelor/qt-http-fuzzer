// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file fuzz_server_winafl.cpp
 * @brief WinAFL fuzzer entry point for black-box fuzz testing of the HTTP
 *        server.
 *
 * This translation unit provides a file-based fuzzing harness compatible with
 * Windows AFL or other custom fuzzing frameworks. It reads input from a file
 * and forwards it to the server's black-box fuzzing interface to stress-test
 * parsing, protocol handling, and internal state transitions.
 *
 * Optional looping mode allows repeated testing of the same input.
 */

#include "server_core.h"  // NOLINT(build/include_subdir)
#include <windows.h>      // NOLINT(build/include_order)

/**
 * @brief File-based fuzzing entry point.
 *
 * This function reads the fuzzed payload from a file and injects it into
 * the HTTP server black-box fuzzing interface.
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
  fuzzServerBlackbox(f_path);
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
    qInfo() << "Usage:" << argv[0] << "<input file> [loop]";
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
