// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file server_core.h
 * @brief Functions for black-box fuzzing of a local QHttpServer.
 *
 * Provides an interface to send raw input bytes to a local HTTP server
 * for black-box fuzzing. The server is automatically initialized on the first
 * call.
 */

#ifndef SRC_SERVER_CORE_H_
#define SRC_SERVER_CORE_H_

#include <cstddef>
#include <cstdint>

/**
 * @brief Sends raw bytes to a local QHttpServer for fuzzing.
 *
 * The server is automatically initialized on the first call.
 *
 * @param data Pointer to raw input bytes.
 * @param size Number of bytes in the input buffer.
 * @return true if the data was successfully sent, false otherwise.
 */
bool fuzzServerBlackbox(const uint8_t *data, size_t size);

#endif  // SRC_SERVER_CORE_H_
