// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file fuzz_server_afl.cpp
 * @brief AFL++ fuzzing harness for black-box testing of the HTTP server.
 *
 * This translation unit reads fuzzed input from standard input (`stdin`) and
 * forwards it to the server's black-box fuzzing interface. Each AFL iteration
 * is forked by the AFL instrumentation to isolate inputs and maintain
 * deterministic behavior.
 *
 * Input constraints are applied to avoid pathological memory usage during
 * fuzzing.
 */

#include "server_core.h"  // NOLINT(build/include_subdir)
#include <stdint.h>       // NOLINT(build/include_order)
#include <unistd.h>       // NOLINT(build/include_order)

/**
 * @brief AFL++ fuzzing main entry point.
 *
 * Reads up to 64 KiB of fuzzed data from `stdin` into a buffer and passes it
 * to the server black-box fuzzing function. Empty inputs or inputs exceeding
 * 64 KiB are ignored.
 *
 * @return Always returns 0.
 *
 * @note The buffer `data` is reused across AFL iterations. Each iteration is
 *       forked by AFL, so there are no side effects between runs.
 */
int main() {
  uint8_t data[65536];

  while (__AFL_LOOP(1000)) {
    const size_t size = read(0, data, sizeof(data));

    // Skip empty or huge inputs
    if (size <= 0 || size > 64 * 1024)
      continue;

    // Inject the fuzzed payload to the HTTP server
    fuzzServerBlackbox(data, size);
  }
  return 0;
}
