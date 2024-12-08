#pragma once

#include "tcpserver.h"
#include "chain.h"

class TcpTester: public TcpServer {
  class TestService : public TcpSocket, public sigc::trackable {
    friend class TcpTester;
    //notify on disconnects, including on destructor
    using TestSlot=sigc::slot<void(TestService *)>;
    TestSlot onDisconnect;

    /** expect this to be called with connected==false only when remote disconnects. */
    void connectionChanged(bool connected);

    u8 bufferAllocation[100];
    ByteScanner buffer;

    void reader(ByteScanner&raw);

    /** called when can write, should set ByteScanner to point to data, and return true if should be sent.*/
    bool writer(ByteScanner&raw);

    /** remote has gone quiet*/
    void goneQuiet();
  public:
    TestService(int fd,u32 ipv4,TestSlot container);

    /** */
    bool disconnect(void);

  };
  ///////////////////////////////
  Chain<TestService> spawned;//todo:1 intermediate class of MultiTcpServer with templated list and factory.

  /** called by @param dead when it loses its remote connection. */
  void disconnect(TestService *dead);
public:
  TcpTester(int port,int backlog=3);

  TcpSocket *spawnClient(int client_fd, u32 ipv4);
  /** BLOCKING server that responds with a fixed message to any message received. */
  bool runMiniServer(int port);
};
