//
// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT
//
// Testing Parser Core
//

#include <QObject>
#include <QString>
#include <QtTest/QTest>

#include "../src/parser_core.h"

class TestParserCore : public QObject {
  Q_OBJECT

  // Tells Qt it is a signal/slot mechanism
private slots:  // NOLINT(whitespace/indent)
  void testValidHttpRequestBytes();
  void testMustFail();
};

/**
 * @brief Valid minimal HTTP GET request as raw bytes should parse successfully.
 */
void TestParserCore::testValidHttpRequestBytes() {
  const char *httpRequest = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "\r\n";

  const bool result = fuzzParserOnly(
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
  const bool result = fuzzParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));
  QVERIFY(!result);
}

QTEST_APPLESS_MAIN(TestParserCore)
#include "test_parser_core.moc"
