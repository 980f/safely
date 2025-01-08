#pragma once

#include "sigcuser.h"
#include "tcpsocket.h"

class TcpServer : SIGCTRACKABLE {
  //@see spawnClient
  using Spawner = sigc::slot<void(int /*fd*/, uint32_t /*ipv4*/)>;

  class ServerSocket : public TcpSocketBase, SIGCTRACKABLE {
    friend class TcpServer;
    ServerSocket(uint32_t remoteAddress = ~0, int port = ~0);
    Spawner spawner; // will be part of TcpServer

    /** @returns whether spawner might get called someday */
    bool accept(int backlog, Spawner spawner);
    /** called when OS gets a connection, will call spawner */
    void incoming();
  } server;
  /** @see bind/listen, number of incoming connections queued for accept.
   * perhaps add to connectArgs, which are inside the ServerSocket.*/
  int backlog;
  /** some internal diagnostics:*/
  int connects;
  int disconnects;

public:
  /** successfully bound etc. */
  bool listening; //
  /** @param interfaceIP selects interface to listen on */
  TcpServer(int port, int interfaceIP = 0, int backlog = 2);
  /** create a non functional one */
  TcpServer();

  /** start listening,includes async accept */
  bool startup();
  bool serveAt(int port, int interfaceIP = 0x7f000001, int backlog = 2);
  /** called when the underlying service accepts a connection
   * @return the socket (so that we can watch its lifecycle), return nullptr if refusing connection */
  virtual TcpSocket *spawnClient(int fd, uint32_t ipv4) = 0;
  /** close all sockets (if possible) and quit serving */
  virtual void shutdown(bool permanently);
  virtual ~TcpServer();
  void onAttach(int fd, uint32_t ipv4);
  bool isConnected();
};
