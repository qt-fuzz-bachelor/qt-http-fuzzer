// Copyright 2026
// SPDX-License-Identifier: MIT

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QHttpServer>
#include <QTcpServer>
#include <QTcpSocket>
#include <windows.h>

static QHttpServer *server = nullptr;
static QTcpServer *tcpserver = nullptr;
static quint16 serverPort = 0;

static void startServer() {
  server = new QHttpServer;

  server->route("/", [](const QHttpServerRequest &) {
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

extern "C" __declspec(dllexport) __declspec(noinline) int
fuzz_target(char *input_file_path) {
  // Read the file AFL wrote into a byte array
  QFile f(input_file_path);
  if (!f.open(QIODevice::ReadOnly)) {
    return 0;
  }
  QByteArray request = f.readAll();

  // Send it raw to the server
  QTcpSocket socket;
  socket.connectToHost("127.0.0.1", serverPort);
  socket.waitForConnected(50);
  socket.write(request);
  socket.waitForBytesWritten(50);
  socket.waitForReadyRead(50);
  socket.disconnectFromHost();
  return 0;
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  qputenv("QT_LOGGING_RULES", "*.debug=false");

  if (argc < 2) {
    qInfo() << "Usage:" << argv[0] << "<input file> [loop]";
    return 0;
  }

  startServer();

  if (argc == 3 && !strcmp(argv[2], "loop")) {
    while (true) {
      fuzz_target(argv[1]);
    }
  } else {
    return fuzz_target(argv[1]);
  }
}
