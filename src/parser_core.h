// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file parser_core.h
 * @brief Interface for fuzzing the QHttpServer parser in isolation.
 *
 * This header declares functions for feeding raw input bytes directly
 * to a QHttpServerParser instance using a QBuffer, allowing testing
 * or fuzzing without starting a full HTTP server.
 */

#ifndef SRC_PARSER_CORE_H_
#define SRC_PARSER_CORE_H_

#include <cstddef>
#include <cstdint>

/**
 * @brief Fuzzes the HTTP parser in isolation.
 *
 * This function feeds raw input bytes to a QHttpServerParser instance
 * using a QBuffer. It allows testing or fuzzing the parser without
 * starting a full HTTP server. Inputs that are empty or exceed 64 KB
 * are ignored.
 *
 * @param data Pointer to the input buffer containing raw bytes.
 * @param size Size of the input buffer in bytes.
 *
 * @return `true` if parsing succeeded, `false` if parsing failed, the
 *         input was invalid, or the buffer could not be opened.
 *
 * @note This function does not modify any external state. Logging
 *       is optional and can be enabled for debugging purposes.
 */
bool fuzzParserOnly(const uint8_t *data, size_t size);

#endif  // SRC_PARSER_CORE_H_
