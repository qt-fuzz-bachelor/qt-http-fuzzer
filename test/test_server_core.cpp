//
// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT
//
// Testing the Server Core
//

#include <QObject>
#include <QString>
#include <QtTest/QTest>

#include "../src/server_core.h"

class TestServerCore : public QObject {
  Q_OBJECT

  // Tells Qt it is a signal/slot mechanism
private slots:  // NOLINT(whitespace/indent)
  void testBasicHttpRequest();
};

// Tests the server with a basic HTTP GET request.

void TestServerCore::testBasicHttpRequest() {
  const char *httpRequest = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "\r\n";

  bool result = fuzzServerBlackbox(
      reinterpret_cast<const uint8_t *>(httpRequest), strlen(httpRequest));

  QVERIFY(result);
}

QTEST_MAIN(TestServerCore);

#include "test_server_core.moc"
