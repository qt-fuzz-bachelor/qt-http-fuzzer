// Copyright 2026
// SPDX-License-Identifier: MIT

#include <QCoreApplication>
#include <QEventLoop>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <stdint.h>
#include <unistd.h>

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

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  startServer();

  uint8_t buf[65536];

  // Persistent
  while (__AFL_LOOP(1000)) {
    ssize_t n = read(0, buf, sizeof(buf));
    if (n <= 0 || n > 64 * 1024)
      continue;

    QTcpSocket sock;
    sock.connectToHost(QHostAddress::LocalHost, serverPort);

    if (!sock.waitForConnected(5))
      continue;

    sock.write(reinterpret_cast<const char *>(buf), n);
    sock.flush();

    for (int i = 0; i < 20; i++) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
      usleep(1000);
    }

    sock.disconnectFromHost();
    sock.waitForDisconnected(5);
  }

  return 0;
}
