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
#include "server_core.h"  // NOLINT(build/include_subdir)
#include <unistd.h>       // NOLINT(build/include_order)

/**
 * @brief AFL++ fuzzing main entry point.
 *
 * Reads up to 64 KiB of fuzzed data from `stdin` into a buffer and passes it
 * to the selected fuzzing interface. Empty inputs or inputs exceeding
 * 64 KiB are ignored.
 *
 * @return Always returns 0.
 *
 * @note The buffer `buf` is reused across AFL iterations. Each iteration is
 *       forked by AFL, ensuring no side effects between runs.
 */
int main() {
  __AFL_FUZZ_INIT();
  const unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;

  while (__AFL_LOOP(1000)) {
    int len = __AFL_FUZZ_TESTCASE_LEN;

    // Skip empty or huge inputs
    if (len <= 0 || len > 64 * 1024)
      continue;

      // Inject the fuzzed payload to the HTTP server
#ifdef FUZZ_HTTP_SERVER
    fuzzServerBlackbox(buf, static_cast<size_t>(len));
#elif defined(FUZZ_HTTP_PARSER)
    fuzzHttpParserOnly(buf, static_cast<size_t>(len));
#elif defined(FUZZ_HEADER_PARSER)
    fuzzHeaderParserOnly(buf, static_cast<size_t>(len));
#else
#error "No fuzzing mode defined! Please define one of " \
       "FUZZ_HTTP_SERVER, FUZZ_HTTP_PARSER, FUZZ_HEADER_PARSER"
#endif
  }
  return 0;
}
