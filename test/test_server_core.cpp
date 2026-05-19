// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file test_server_core.cpp
 * @brief Unit and fuzz tests for the HTTP server core.
 *
 * This file contains Qt test cases for validating HTTP server behavior in
 * isolation. It covers basic request handling, malformed input, edge cases, and
 * testing using previously discovered crash-inducing inputs.
 *
 * Tests operate via fuzzServerBlackbox() without requiring a real network
 * setup.
 */

#include <QObject>
#include <QString>
#include <QtTest/QTest>

#include "server_core.h"  // NOLINT(build/include_subdir)

class TestServerCore : public QObject {
  Q_OBJECT

private slots:  // NOLINT(whitespace/indent)
  void testServerInitialization();
  void testBasicHttpRequest();
  void testEmptyData();
  void testMalformedRequest();
  void testCrashFiles();
  void testCrashFiles_data();
};

/**
 * @brief Verifies that the server accepts an initial connection request.
 *
 * This test sends a minimal input ("GET") to confirm that the server is running
 * and able to receive incoming data.
 */
void TestServerCore::testServerInitialization() {
  const char *minimalRequest = "GET";

  bool result =
      fuzzServerBlackbox(reinterpret_cast<const uint8_t *>(minimalRequest),
                         strlen(minimalRequest));

  QVERIFY(result);
}

/**
 * @brief Validates handling of a well-formed HTTP GET request.
 *
 * This test sends a complete HTTP/1.1 request and verifies that the server
 * correctly processes standard valid input.
 */
void TestServerCore::testBasicHttpRequest() {
  const char *httpRequest = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "\r\n";

  bool result = fuzzServerBlackbox(
      reinterpret_cast<const uint8_t *>(httpRequest), strlen(httpRequest));

  QVERIFY(result);
}

/**
 * @brief Ensures the server handles empty input safely.
 *
 * This test provides a null pointer and zero-length input to verify that the
 * server does not crash and handles empty requests gracefully.
 */
void TestServerCore::testEmptyData() {
  bool result = fuzzServerBlackbox(nullptr, 0);
  QVERIFY(result);
}

/**
 * @brief Tests server robustness against malformed HTTP input.
 *
 * This test sends a deliberately invalid request format to verify that the
 * server can handle corrupted or non-standard input without failure.
 */
void TestServerCore::testMalformedRequest() {
  const char *malformedRequest = "GET\r\n\r\nHTTP/42\r\n";

  bool result =
      fuzzServerBlackbox(reinterpret_cast<const uint8_t *>(malformedRequest),
                         strlen(malformedRequest));

  QVERIFY(result);
}

/**
 * @brief Loads and tests previously discovered crash-inducing inputs.
 *
 * This data-driven test reads input files from the `crashes` directory and
 * feeds each one into the server. It ensures regression stability and prevents
 * reintroduction of known failure cases.
 */
void TestServerCore::testCrashFiles_data() {
  QTest::addColumn<QByteArray>("request");

  QDir dir(QString(SRCDIR));
  dir.cd("crashes");

  for (const QString &fileName : dir.entryList(QDir::Files)) {
    QFile file(dir.filePath(fileName));
    if (!file.open(QIODevice::ReadOnly))
      continue;

    QByteArray data = file.readAll();

    QTest::newRow(fileName.toUtf8().constData()) << data;
  }
}

/**
 * @brief Executes regression tests using stored crash inputs.
 *
 * Each input from the crash corpus is passed to the server to ensure it remains
 * stable and does not crash on previously problematic data.
 */
void TestServerCore::testCrashFiles() {
  QFETCH(QByteArray, request);

  bool result = fuzzServerBlackbox(
      reinterpret_cast<const uint8_t *>(request.constData()), request.size());

  QVERIFY(result);
}

QTEST_APPLESS_MAIN(TestServerCore)
#include "test_server_core.moc"
