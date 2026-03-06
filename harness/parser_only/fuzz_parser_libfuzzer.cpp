// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file fuzz_parser_libfuzzer.cpp
 * @brief libFuzzer fuzzing harness for isolated fuzzing of the HTTP parser.
 *
 * This translation unit provides the required entry function for LLVM
 * libFuzzer. The fuzzer forwards arbitrary input data to the isolated parser
 * fuzzing interface in order to stress-test parsing.
 *
 * Input constraints are applied to avoid pathological memory usage during
 * fuzzing.
 */

#include "parser_core.h"  // NOLINT(build/include_subdir)

/**git
 * @brief libFuzzer test entry point.
 *
 * This function is called repeatedly by LLVM libFuzzer with mutated input data.
 * Each invocation must be deterministic and must not retain state between runs
 * unless explicitly intended by the fuzzing strategy.
 *
 * The function filters out:
 * - Empty inputs (no meaningful execution path)
 * - Oversized inputs (> 64 KiB) to prevent excessive memory consumption
 *
 * The remaining payload is forwarded to the http parser fuzzing interface.
 *
 * @param data Pointer to the fuzzed input buffer (not null unless size == 0).
 * @param size Size of the fuzzed input buffer in bytes.
 *
 * @return Always returns 0 as required by libFuzzer.
 */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, const size_t size) {
  // Skip empty or huge inputs
  if (size == 0 || size > 64 * 1024)
    return 0;

  // Inject the fuzzed payload to the HTTP parser
  fuzzParserOnly(data, size);
  return 0;
}
