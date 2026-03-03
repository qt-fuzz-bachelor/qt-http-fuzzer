// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file parser_core.cpp
 * @brief Implementation of the HTTP parser fuzzing functions.
 *
 * This file contains functions for fuzzing QHttpServerParser in isolation.
 * It allows raw byte input to be fed into the parser for black-box fuzzing
 * or unit testing without starting a full HTTP server.
 *
 * @note Logging is done via qDebug/qCritical and can be enabled or disabled.
 */

#include "parser_core.h"  // NOLINT(build/include_subdir)
#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QtHttpServer/private/qhttpserverparser_p.h>
#include <QtHttpServer/private/qhttpserverrequestfilter_p.h>

/**
 * @brief Fuzzes the QHttpServer parser with raw input bytes.
 *
 * This function creates a temporary QHttpServerParser instance and injects
 * the provided raw input data into it using a QBuffer. It allows testing
 * the parser in isolation from the full HTTP server, which is useful for
 * fuzzing or unit testing malformed or unexpected HTTP input.
 *
 * @param data Pointer to a buffer containing raw input bytes to parse.
 * @param size Size of the input buffer in bytes.
 *
 * @return `true` if the parser successfully processed the input, `false`
 *         if parsing failed or a QBuffer
 *         could not be opened.
 *
 * @note This function does not modify any external state. Logging is done
 *       using qDebug/qCritical, which can be enabled or disabled as needed.
 */
bool fuzzParserOnly(const uint8_t *data, size_t size) {
  // Create filter and parser
  QHttpServerRequestFilter filter;
  QHttpServerParser parser{QHostAddress{}, 8080, QHostAddress{}, 8080, &filter};

  // Inject raw bytes into a buffer
  QByteArray byteArray{reinterpret_cast<const char *>(data),
                       static_cast<int>(size)};
  QBuffer buffer{&byteArray};
  if (!buffer.open(QBuffer::ReadOnly)) {
    qCritical() << "Failed to open QBuffer";
    return false;
  }

  // Parse the raw bytes
  bool result = parser.parse(&buffer);

  if (!result) {
    // Optional: logging during unit tests
    qDebug() << "Parser failed on input of size" << size;
  }

  return result;
}
