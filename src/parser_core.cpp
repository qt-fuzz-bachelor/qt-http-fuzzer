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
#include <QFile>
#include <QHostAddress>
#include <QIODevice>
#include <QtHttpServer/private/qhttpserverparser_p.h>
#include <QtHttpServer/private/qhttpserverrequestfilter_p.h>
#include <QtNetwork/private/qhttpheaderparser_p.h>

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

/**
 * @brief Fuzzes the QHttpHeaderParser with raw input bytes.
 *
 * This function creates a temporary QHttpHeaderParser instance and injects
 * the provided raw input data into it using a QBuffer. It allows testing
 * the header parser in isolation from the full HTTP server, which is useful
 * for fuzzing or unit testing malformed or unexpected HTTP header input.
 *
 * @param data Pointer to a buffer containing raw input bytes to parse.
 * @param size Size of the input buffer in bytes.
 *
 * @return `true` if the parser successfully processed the input, `false`
 *         if parsing failed, the input size exceeded the supported range,
 *         or a QBuffer could not be opened.
 *
 * @note This function does not modify any external state. Logging is done
 *       using qDebug/qCritical, which can be enabled or disabled as needed.
 */
bool fuzzHeaderParserOnly(const uint8_t *data, size_t size) {
  if (size > std::numeric_limits<int>::max())
    return false;

  QHttpHeaderParser parser;

  QByteArray byteArray(reinterpret_cast<const char *>(data),
                       static_cast<int>(size));

  // Directly pass a view of the data
  bool result = parser.parseHeaders(QByteArrayView(byteArray));

  if (!result) {
    qDebug() << "Parser failed on input of size" << size;
  }

  return result;
}

/**
 * @brief Fuzzes the QHttpServer parser with raw input read from a file.
 *
 * This function reads the entire content of the given file into a QBuffer
 * and feeds it to a temporary QHttpServerParser instance. It is useful
 * for fuzzing or testing the parser against stored input files.
 *
 * @param file_path Path to the file containing raw input bytes.
 * @return true if the parser successfully processed the input, false
 *         if parsing failed or the file/QBuffer could not be opened.
 *
 * @note Logging is performed via qDebug/qCritical. This function does not
 *       modify any external state.
 */
bool fuzzParserOnly(const char *file_path) {
  QHttpServerRequestFilter filter;
  QHttpServerParser parser{QHostAddress{}, 8080, QHostAddress{}, 8080, &filter};

  QFile f(file_path);
  if (!f.open(QIODevice::ReadOnly))
    return false;

  // Inject raw bytes from file into a buffer
  QByteArray byteArray = f.readAll();
  QBuffer buffer{&byteArray};
  if (!buffer.open(QBuffer::ReadOnly)) {
    qCritical() << "Failed to open QBuffer";
    return false;
  }

  return parser.parse(&buffer);
}
