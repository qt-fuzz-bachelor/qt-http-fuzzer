// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file fuzz_parser_afl.cpp
 * @brief AFL++ fuzzing harness for isolated fuzzing of the HTTP parser.
 *
 * This translation unit reads fuzzed input from standard input (`stdin`) and
 * forwards it to the HTTP parser's isolated fuzzing interface. Each AFL
 * iteration is forked by the AFL instrumentation to isolate inputs and maintain
 * deterministic behavior.
 *
 * Input constraints are applied to avoid pathological memory usage during
 * fuzzing.
 */

#include "parser_core.h"  // NOLINT(build/include_subdir)
#include <stdint.h>       // NOLINT(build/include_order)
#include <unistd.h>       // NOLINT(build/include_order)

/**
 * @brief AFL++ fuzzing main entry point.
 *
 * Reads up to 64 KiB of fuzzed data from `stdin` into a buffer and passes it
 * to the HTTP parser for isolated fuzzing. Empty inputs or inputs exceeding
 * 64 KiB are ignored.
 *
 * @return Always returns 0.
 *
 * @note The buffer `data` is reused across AFL iterations. Each iteration is
 *       forked by AFL, ensuring no side effects between runs.
 * @note The fuzzing process directly targets the HTTP parser by passing the raw
 *       input data to `fuzzParserOnly`, isolating the parser from the full HTTP
 *       server stack for testing malformed or unexpected input.
 */
int main() {
  uint8_t data[65536];

  while (__AFL_LOOP(1000)) {
    const size_t size = read(0, data, sizeof(data));

    // Skip empty or huge inputs
    if (size <= 0 || size > 64 * 1024)
      continue;

    // Inject the fuzzed payload to the HTTP server
    fuzzParserOnly(data, size);
  }
  return 0;
}
