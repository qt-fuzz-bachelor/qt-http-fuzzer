//
// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT
//
// Testing Server Core
//

#include "server_core.h"  // NOLINT(build/include_subdir)
#include <QObject>        // NOLINT(build/include_order)
#include <QString>        // NOLINT(build/include_order)
#include <QtTest/QTest>   // NOLINT(build/include_order)

class TestServerCore : public QObject {
  Q_OBJECT

  // Tells Qt it is a signal/slot mechanism
private slots:  // NOLINT(whitespace/indent)
  void testServerInitialization();
  void testBasicHttpRequest();
  void testEmptyData();
  void testMalformedRequest();
  void testCrashFiles();
  void testCrashFiles_data();
};

/**
 * @brief Tests that the server initializes and accepts connections.
 *
 * Verifies the server starts up correctly by sending a minimal request.
 * This ensures the server is actually listening and can receive data.
 */
void TestServerCore::testServerInitialization() {
  const char *minimalRequest = "GET";
  bool result =
      fuzzServerBlackbox(reinterpret_cast<const uint8_t *>(minimalRequest),
                         strlen(minimalRequest));
  QVERIFY(result);
}

/**
 * @brief Tests the server with a complete HTTP GET request.
 *
 * Sends a properly formatted HTTP request to verify the server
 * can handle valid HTTP protocol messages.
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
 * @brief Tests the server with nullptr input.
 *
 * Verifies the server handles edge cases gracefully without crashing.
 * Empty data should still return true (connection succeeded even if no data).
 */
void TestServerCore::testEmptyData() {
  bool result = fuzzServerBlackbox(nullptr, 0);
  QVERIFY(result);
}

void TestServerCore::testMalformedRequest() {
  const char *malformedRequest = "GET\r\n\r\nHTTP/42\r\n";
  bool result =
      fuzzServerBlackbox(reinterpret_cast<const uint8_t *>(malformedRequest),
                         strlen(malformedRequest));
  QVERIFY(result);
}

void TestServerCore::testCrashFiles_data() {
  QTest::addColumn<QByteArray>("request");

  QDir dir(QString(SRCDIR) + "/crashes");

  for (const QString &fileName : dir.entryList(QDir::Files)) {
    QFile file(dir.filePath(fileName));
    if (!file.open(QIODevice::ReadOnly))
      continue;

    QByteArray data = file.readAll();

    // Each file becomes its own row in the test
    QTest::newRow(fileName.toUtf8().constData()) << data;
  }
}

void TestServerCore::testCrashFiles() {
  QFETCH(QByteArray, request);

  bool result = fuzzServerBlackbox(
      reinterpret_cast<const uint8_t *>(request.constData()), request.size());

  QVERIFY(result);
}

QTEST_APPLESS_MAIN(TestServerCore)
#include "test_server_core.moc"
