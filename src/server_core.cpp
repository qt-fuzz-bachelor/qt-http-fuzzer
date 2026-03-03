// Copyright (c) 2026 T.W. Skårer, J.F. Wilvang, S. Thire
// SPDX-License-Identifier: MIT

/**
 * @file server_core.cpp
 * @brief Implementation of black-box HTTP server fuzzing functions.
 *
 * This file contains functions to initialize a local QHttpServer for fuzzing
 * and to send raw input bytes as black-box fuzz tests. The server responds
 * with "ok" to all requests and runs entirely on the loopback interface.
 *
 * @note Logging is done via qDebug/qCritical and can be enabled or disabled.
 */

#include "server_core.h"  // NOLINT(build/include_subdir)
#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QHostAddress>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QTcpServer>
#include <QTcpSocket>

namespace {
QHttpServer *server = nullptr;    // Pointer to the QHttpServer instance
QTcpServer *tcpserver = nullptr;  // Underlying TCP server
quint16 serverPort = 0;           // Port assigned by QTcpServer
bool initialized = false;         // Whether the server has been initialized

/**
 * @brief Initializes a local QHttpServer instance for fuzzing.
 *
 * This function is called automatically by fuzzServerBlackbox() if
 * the server hasn't been initialized yet. It sets up a catch-all
 * route that responds with "ok" and binds the server to a random
 * free port on the loopback interface.
 *
 * Only meant to be used internally within this file.
 */
void initializeServer() {
  if (initialized)
    return;  // Avoid re-initializing

  int argc = 0;
  char **argv = nullptr;
  new QCoreApplication(argc, argv);

  // Create QHttpServer instance
  server = new QHttpServer;

  // Add a catch-all route that responds "ok" to every request
  server->route("/*", [](const QHttpServerRequest &) {
    return QHttpServerResponse("ok");
  });

  // Create a TCP server on loopback with an open port
  tcpserver = new QTcpServer();
  if (!tcpserver->listen(QHostAddress::LocalHost, 0)) {
    qFatal("Failed to listen");
  }

  // Bind the HTTP server to the TCP server
  if (!server->bind(tcpserver)) {
    qFatal("Failed to bind QHttpServer");
  }

  // Store the dynamically assigned port
  serverPort = tcpserver->serverPort();
  initialized = true;

  // Optional debug info
  qDebug() << "Server started on port" << serverPort;
}
}  // namespace

/**
 * @brief Sends raw input bytes to the local QHttpServer for black-box fuzzing.
 *
 * If the server isn't initialized yet, it will be initialized automatically.
 * This function connects to the server via QTcpSocket, writes the data,
 * flushes the socket, and gives the server a short time to process.
 *
 * The maximum time waited for the TCP connection and disconnection is limited
 * to 10 milliseconds to keep fuzzing iterations fast.
 *
 * @param data Pointer to raw input bytes to send.
 * @param size Number of bytes in the input buffer.
 * @return true if the data was successfully sent, false otherwise.
 */
bool fuzzServerBlackbox(const uint8_t *data, size_t size) {
  // Ensure server is initialized
  if (!initialized)
    initializeServer();

  QTcpSocket sock;
  sock.connectToHost(QHostAddress::LocalHost, serverPort);

  // Give up if unable to connect
  if (!sock.waitForConnected(10)) {
    qDebug() << "Failed to connect to server";
    return false;
  }

  // Send raw bytes over TCP
  sock.write(reinterpret_cast<const char *>(data), size);
  sock.flush();

  // Let QHttpServer process the request
  while (QCoreApplication::hasPendingEvents())
    QCoreApplication::processEvents(QEventLoop::AllEvents);

  // Disconnect gracefully
  sock.disconnectFromHost();
  sock.waitForDisconnected(10);

  return true;
}
