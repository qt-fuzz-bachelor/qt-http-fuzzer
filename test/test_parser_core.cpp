//
// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT
//
// Testing Parser Core
//

#include "parser_core.h"  // NOLINT(build/include_subdir)
#include <QObject>        // NOLINT(build/include_order)
#include <QString>        // NOLINT(build/include_order)
#include <QtTest/QTest>   // NOLINT(build/include_order)

class TestParserCore : public QObject {
  Q_OBJECT

  // Tells Qt it is a signal/slot mechanism
private slots:  // NOLINT(whitespace/indent)
  void testValidHttpRequestBytes();
  void testMustFail();
  void testRandome();
};

/**
 * @brief Valid minimal HTTP GET request as raw bytes should parse successfully.
 */
void TestParserCore::testValidHttpRequestBytes() {
  const char *httpRequest = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "\r\n";

  const bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));

  QVERIFY(result);
}

/**
 * @brief Verifies that the HTTP parser rejects an invalid request-line.
 *
 * This test ensures that QHttpServerParser::parse() returns false when the
 * request-line contains an invalid protocol/version token.
 */
void TestParserCore::testMustFail() {
  const char *httpRequest = "GET / HTTX/1.1\r\n"
                            "\r\n";
  const bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));
  QVERIFY(!result);
}

/**
 * @brief Test the parser on a randome string
 *
 *  The parser returns true and does not encounter
 *  a condition it classifies as an error.
 */
void TestParserCore::testRandome() {
  const char *httpRequest = "42123456789abcdHEIPÅDEG";

  const bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));

  QVERIFY(result);
}

QTEST_APPLESS_MAIN(TestParserCore)
#include "test_parser_core.moc"
