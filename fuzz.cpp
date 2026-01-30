// Copyright 2026 Wilvang
// SPDX-License-Identifier: MIT

#include <QCoreApplication>
#include <QEventLoop>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

static QHttpServer *server = nullptr;
static QTcpServer *tcpserver = nullptr;
static quint16 serverPort = 0;

static void startServer() {
  server = new QHttpServer;

  server->route("/*", [](const QHttpServerRequest &) {
    return QHttpServerResponse("ok");
  });

  tcpserver = new QTcpServer();
  if (!tcpserver->listen(QHostAddress::LocalHost, 0)) {
    qFatal("Failed to listen");
  }

  if (!server->bind(tcpserver)) {
    qFatal("Failed to bind http server");
  }

  serverPort = tcpserver->serverPort();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, const qint64 size) {
  static bool initialized = false;

  if (!initialized) {
    int argc = 0;
    char **argv = nullptr;
    new QCoreApplication(argc, argv);
    startServer();
    initialized = true;
  }

  // Avoid pathological inputs
  if (size == 0 || size > 64 * 1024)
    return 0;

  QTcpSocket sock;
  sock.connectToHost(QHostAddress::LocalHost, serverPort);
  if (!sock.waitForConnected(5))
    return 0;

  sock.write(reinterpret_cast<const char *>(data), size);
  sock.flush();

  // Give Qt time to parse
  QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
  sock.disconnectFromHost();

  return 0;
}
