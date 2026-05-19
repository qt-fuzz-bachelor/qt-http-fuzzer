// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file test_parser_core.cpp
 * @brief Unit tests for the HTTP parser core.
 *
 * This file contains Qt test cases for validating the HTTP parser in isolation.
 * It covers basic correctness tests, malformed input handling, and tests using
 * previously discovered crash inputs.
 *
 * Tests operate directly on QHttpServerParser via fuzzHttpParserOnly(),
 * without running a full HTTP server.
 */

#include <QObject>
#include <QString>
#include <QtTest/QTest>

#include "parser_core.h"  // NOLINT(build/include_subdir)

class TestParserCore : public QObject {
  Q_OBJECT

private slots:  // NOLINT(whitespace/indent)
  void testValidHttpRequestBytes();
  void testMustFail();
  void testJunk();
  void testCrashFiles_data();
  static void testCrashFiles();
};

/**
 * @brief Verifies that a minimal valid HTTP/1.1 GET request is parsed
 * successfully.
 *
 * This test feeds a well-formed HTTP request into the fuzzed HTTP parser and
 * ensures that it is accepted as valid input.
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
 * @brief Ensures that the parser rejects malformed HTTP request-line input.
 *
 * This test verifies that QHttpServerParser::parse() fails when the HTTP
 * version token is invalid or does not conform to the expected format.
 */
void TestParserCore::testMustFail() {
  const char *httpRequest = "GET / HTTX/1.1\r\n"
                            "\r\n";

  const bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));

  QVERIFY(!result);
}

/**
 * @brief Fuzz test using arbitrary non-HTTP input data.
 *
 * This test feeds a random byte sequence into the HTTP parser to evaluate
 * robustness against malformed or unexpected input. The primary goal is to
 * ensure the parser does not crash or exhibit undefined behavior.
 */
void TestParserCore::testJunk() {
  const char *httpRequest = "42123456789abcdHEIPÅDEG";

  const bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(httpRequest), std::strlen(httpRequest));

  QVERIFY(result);
}

/**
 * @brief Loads a corpus of previously discovered crash-inducing inputs.
 *
 * This data-driven test reads all files in the `crashes` directory and supplies
 * each file as an independent test case. It is used for regression testing to
 * ensure that previously failing inputs remain stable.
 */
void TestParserCore::testCrashFiles_data() {
  QTest::addColumn<QByteArray>("request");

  QDir dir(QString(SRCDIR));
  dir.cd("crashes");

  for (const QString &fileName : dir.entryList(QDir::Files)) {
    QFile file(dir.filePath(fileName));
    if (!file.open(QIODevice::ReadOnly))
      continue;

    QByteArray data = file.readAll();

    // Each file becomes its own test row
    QTest::newRow(fileName.toUtf8().constData()) << data;
  }
}

/**
 * @brief Executes fuzz regression tests using previously collected crash
 * inputs.
 *
 * This test retrieves each input generated in testCrashFiles_data() and feeds
 * it into the HTTP parser to ensure it does not crash and behaves consistently
 * across versions.
 */
void TestParserCore::testCrashFiles() {
  QFETCH(QByteArray, request);

  bool result = fuzzHttpParserOnly(
      reinterpret_cast<const uint8_t *>(request.constData()), request.size());

  QVERIFY(result);
}

QTEST_APPLESS_MAIN(TestParserCore)
#include "test_parser_core.moc"
