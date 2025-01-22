#include "tcpsocket.h"
#include <cstdio>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "cheaptricks.h"
#include "fdset.h"
#include "logger.h"
#include "netinet/tcp.h"

Logger spamme("TcpSocket", false);

void TcpSocketBase::connectionEvent(bool connected) {
  if (connected) {
    ++stats.connects;
  } else {
    ++stats.disconnects;
    spamme("Disconnected.");
  }
}

TcpSocketBase::TcpSocketBase(int fd, uint32_t remoteAddress, int port) :connectArgs(remoteAddress, port), sock("TcpSocketBase", fd), source{sock} {}

TcpSocketBase::~TcpSocketBase() = default; //contained classes do any automatic closing needed.

bool TcpSocketBase::isConnected() const {
  return sock.isOpen();
}

////

TcpSocket::TcpSocket(int fd, uint32_t remoteAddress, int port) :
    TcpSocketBase(fd, remoteAddress, port),
    autoConnect(false),
    eagerToWrite(0),
    newConnections(0),
    connectionInProgress(false) {
  whenConnectionChanges(MyHandler(TcpSocket::connectionEvent), false); // use notifier to manage connection counters
  if (sock.isOpen()) {
    startReception();
  }
}

uint32_t TcpSocketBase::remoteIpv4() {
  return connectArgs.ipv4;
}

void TcpSocketBase::disconnect(bool ignored) {
  source.disconnect();
  if (isConnected()) { // #checking for debug purposes
    sock.close();
  }
}

//////////////////////////////

bool TcpSocket::connect(unsigned ipv4, unsigned port, bool noDelay, bool block) {
  connectArgs.ipv4 = ipv4;
  connectArgs.port = port;
  connectArgs.noDelay = noDelay;
  connectArgs.block = block;

  autoConnect = connectArgs.isPossible();
  return autoConnect && reconnect();
}

void TcpSocket::startReception() {
  // source.incoming= sock.input(MyHandler(TcpSocket::readable));
  // source.hangup= sock.hangup(MyHandler(TcpSocket::hangup));
}

void TcpSocket::connectionFailed(int error) {
  stats.lastWrote = -error;
  if (connectArgs.block) {
    dbg("Connection Failed after %g seconds", connectionTimer.elapsed());
    // todo:0 ?if fail immediately then spin some before admitting defeat?
  }
  disconnect(true);
}

bool TcpSocket::reconnect() {
  bool wasConnected = isConnected();
  if (wasConnected) {
    disconnect(false); // don't notify since notify is likely to call connect and hence infinite loop.
  }
  sock.preopened(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
  if (sock.isOpen()) {
    // Setting the socket to reuse the address if we fail and restart
    sock.setOption(SOL_SOCKET, SO_REUSEADDR, int(1));

    if(wasConnected) {
      ++stats.disconnects;//failure to make socket, so number in diags grows if problem persists.
      notifyConnected(false);
    }
  } else {
    return false;
  }

  if (connectArgs.noDelay) {
    spamme("Setting TCP No Delay on");
    if (int ernum = sock.setOption(IPPROTO_TCP, TCP_NODELAY, 1)) {
      dbg("Set TCP no delay returned %d", ernum);
    }
  }
  ++stats.connects;

  // todo:1 research whether the default so_linger is false, we'd like a reset() rather than a gentle close() when we disconnect.
  SocketAddress sad(connectArgs); // :(
  connectionTimer.start();
  if (int error = sad.connect(sock)) {
    switch (error) {
      case EALREADY: // todo:1 research why we would ever get this
        break;
      case EINPROGRESS:
        break; // treat the same as a return of 0
      case ECONNREFUSED:
        // #join
      default:
        connectionFailed(error);
        return false;
    }
  }
  // following dbg()s will spam with useless information unless you're debugging socket stuff
  if (connectArgs.block) { // new way
    // dbg("TCPsocket Waiting for positive connection before sending data");
    connectionInProgress = true;
    writeInterest();
  } else { // old way: let connection errors fold into write errors
    // dbg("TCPsocket doing legacy sloppy connection, will be spamming data while connecting is in progress");
    connectionInProgress = false; // in case we change 'block' after some gross failure
    startReception();
    notifyConnected(true);
  }
  return true; // we didn't fail, doesn't mean 'actually connected'
}

void TcpSocket::disconnect(bool andNotify) {
  TcpSocketBase::disconnect(andNotify);
  if (andNotify) {
    notifyConnected(false);
  }

}

void TcpSocket::flush() {
  // there is no flush in TCP.
  uint8_t bytes[4096] = {0};//at least one null in case we treat this as a string?
  // can we stat a socket fd?
  int notforever = 10000;
  while (sock.read(bytes, sizeof(bytes)) && sock.lastRead == sizeof(bytes)) { // todo:00 inform library we imported this from of its gross error!
    // a debug message here might slow us down enough to never catch up with the source and hence make this an infinite loop.
    if (--notforever <= 0) {
      return;
    }
  }
}

bool TcpSocket::readable() {
  uint8_t bytes[4096] = {0};

  if (!sock.read(bytes, sizeof(bytes))) { // todo:00 glib version had bad bug here
    dbg("read err: %d on %08X:%d", -stats.lastRead, connectArgs.ipv4, connectArgs.port); // useful for debug, but it will spam you
    disconnect(true);
    return false;
  } else if (stats.lastRead == 0) { // read returns zero when a client cleanly disconnects
    dbg("The remote client has disconnected");
    disconnect(true);
    return false;
  } else { // must be>0
    ByteScanner chunk(bytes, stats.lastRead);
    ++stats.reads;
    reader(chunk);
  }
  return true;
}

bool TcpSocket::writeable() {
  if (flagged(connectionInProgress)) { // then we are finishing up connecting
    connectionTimer.stop();
    dbg("Completing connection.");
    int socketError(0);
    if (int error = sock.getOption(SOL_SOCKET, SO_ERROR, socketError)) {
      connectionFailed(error);
      return false;
    }
    // #leave this and the above separate for debug.
    if (socketError) {
      connectionFailed(socketError);
      return false;
    }
    startReception();
    notifyConnected(true);
    return eagerToWrite;
  }
  // else socket is ready for write data
  eagerToWrite = 0;
  if (sendbuf.hasNext() || writer(sendbuf)) { // if have data in hand else call writer() to see if it wants to add more and if so then ...
    if (!sock.write(sendbuf)) { // todo:00 glib instance had bad bug here
      connectionFailed(-stats.lastWrote);
      dbg("write errno: %d on %08X:%d", -stats.lastWrote, connectArgs.ipv4, connectArgs.port);
      return false; // failure, lose data, drop connection and sleep until reconnected and writeInterest is called.
    }
    if (stats.lastWrote > 0) {
      ++stats.writes; // logs attempts
      bool more = sendbuf.hasNext(); // often false
      if (more) {
        ++eagerToWrite;
      }
      return more;
    } else {
      // wtf? shouldn't be possible
      return true; // more please
    }
  } else {
    sendbuf.dump(); // in case writer() was sloppy.
    return false; // nothing to send, sleep until writeInterest is called.
  }
}

/** expect this when far end of socket spontaneously closes*/
bool TcpSocket::hangup() {
  disconnect(true);
  return false;
}

void TcpSocket::writeInterest() {
  ++eagerToWrite; // we actually do want to send data
  if (!connectionInProgress) { // in case we use a separate callback for connection.
    if (source.writeInterest(sigc::hide_return(MyHandler(TcpSocket::writeable)))) {
      ++newConnections;
    }
  }
}

TcpSocket::~TcpSocket() {
  disconnect(false);
}

sigc::connection TcpSocket::whenConnectionChanges(const BooleanSlot &nowConnected, bool kickme) {
  if (kickme) {
    nowConnected(isConnected());
  }
  return notifyConnected.connect(nowConnected);
}
///////////////////////////
TcpSocketBase::Stats::Stats() {
  clear();
}

void TcpSocketBase::Stats::clear() {
  connects = 0;
  disconnects = 0;
  lastRead = 0;
  lastWrote = 0;
  reads = 0;
  writes = 0;
}

///////////////////////////
bool TcpSocket::ConnectArgs::isPossible() {
  return port || ipv4;//non zero port on 0.0.0.0 or port 0 on any other IP address.
}

TcpSocket::ConnectArgs::ConnectArgs(int ipv4, int port, bool noDelay, bool block) :
    ipv4(ipv4),
    port(port),
    noDelay(noDelay),
    block(block) {
  // #nada
}

void TcpSocket::ConnectArgs::erase() {
  ipv4 = 0;
  port = 0;
  noDelay = false;
}

//////////////////////////////
SocketAddress::SocketAddress() {
  EraseThing(sin);
}

SocketAddress::SocketAddress(TcpSocket::ConnectArgs &cargs) {
  EraseThing(sin);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(cargs.ipv4);
  sin.sin_port = htons(cargs.port);
}

uint16_t SocketAddress::hostPort() const {
  return ntohs(sin.sin_port);
}

uint32_t SocketAddress::hostAddress() const {
  return ntohl(sin.sin_addr.s_addr);
}

const sockaddr *SocketAddress::addr() const {
  return reinterpret_cast<const sockaddr *>(&sin);
}

sockaddr *SocketAddress::addr() {
  return reinterpret_cast<sockaddr *>(&sin);
}

int SocketAddress::len() const {
  return sizeof(sin);
}

int SocketAddress::connect(int fd) {
  return ::connect(fd, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)) ? errno : 0;
}

BlockingConnectSocket::BlockingConnectSocket(int fd, uint32_t remoteAddress, int port) :
    TcpSocketBase(fd, remoteAddress, port),
    autoConnect(false) {
  if (sock.isOpen()) {
    startReception();
  }
}

BlockingConnectSocket::~BlockingConnectSocket() {
  disconnect(false);
}

void BlockingConnectSocket::startReception() {
  source.listen(sigc::hide_return(MyHandler(BlockingConnectSocket::readable)), MyHandler(BlockingConnectSocket::hangup));
}

void BlockingConnectSocket::disconnect(bool notify) {
  TcpSocketBase::disconnect(notify);
  if (notify) {
    ++stats.disconnects;
    notifyConnected(false);
  }
}

void BlockingConnectSocket::hangup() {
  disconnect(true);
}

bool BlockingConnectSocket::readable() {
  uint8_t bytes[4096] = {0};

  stats.lastRead = sock.read(bytes, sizeof(bytes));
  if (stats.lastRead < 0) {
    //    dbg("read err: %d on %08X:%d",-stats.lastRead,connectArgs.ipv4, connectArgs.port); //useful for debug, but it will spam you
    disconnect(true);
    return false;
  }
  if (stats.lastRead == 0) { // read returns zero when a client cleanly disconnects
    dbg("The remote client has disconnected");
    disconnect(true);
    return false;
  }
  // last read > 0
  // if(stats.lastRead > 0) {//skip 0 to get better stats. NO! zero is returned for a closed connection
  ByteScanner chunk(bytes, stats.lastRead);
  ++stats.reads += 1;
  reader(chunk);
  //}
  return true;
}

bool BlockingConnectSocket::connect(unsigned ipv4, unsigned port) {
  connectArgs.ipv4 = ipv4;
  connectArgs.port = port;
  autoConnect = connectArgs.isPossible();
  return autoConnect && reconnect();
}

// This method will emulate a "timeout" on the connect
bool BlockingConnectSocket::reconnect() {
  bool wasConnected = isConnected();
  if (wasConnected) {
    disconnect(false); // don't notify since notify is likely to call connect and hence infinite loop.
  }
  sock.preopened(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));

  if (!sock.isOpen()) {
    return false;
  }
  int optval(1);

  // Setting the socket to reuse the address if we fail and restart
  sock.setOption(SOL_SOCKET, SO_REUSEADDR, optval);

  if (wasConnected) {
    ++stats.disconnects; // failure to make socket, make the number in diags grow while problem persists.
    notifyConnected(false);
  }
  // #setting the following should probaby be optional.
  //   if(sock.setTCPopt(TCP_NODELAY, 1)){
  //     dbg("Set TCP no delay returned %d",errno);
  //   }

  // todo:1 research whether the default so_linger is false, we'd like a reset()
  //  rather than a gentle close() when we disconnect.
  SocketAddress sad(connectArgs);
  if (sad.connect(sock.asInt())) {
    // we did not immediately connect!  now emulate blocking socket with timeout!
    if (EINPROGRESS == errno) {
      SelectorSet selector;
      // socklen_t lon;
      selector.setTimeout(3.0); ////todo:1 symbol or class option for this timeout
      while (true) {//#spins on a select().
        selector.include(sock);
        // FD_SET(sock.asInt(), &myset);
        int result = selector.select("w");
        if (result < 0 && sock.errornumber != EINTR) {
          disconnect(true);
          return false;
        } else if (result > 0) {
          int valopt(0);//0 for debug
          if (sock.getOption(SOL_SOCKET, SO_ERROR, valopt) && valopt < 0) { // todo:1 can we fale to get SO_ERROR?
            disconnect(true);
            return false;
          }
          // Check the value returned...
          if (valopt) {
            disconnect(true);
            return false;
          }
          break;
        } else {
          fprintf(stderr, "Timeout in select() - Cancelling!\n");
          disconnect(true);
          return false;
        }
      }
    } else { // not a "EINPROGRESS"
      disconnect(true);
      return false;
    }
  }

  ++stats.connects;
  startReception();
  notifyConnected(true);
  return true;
}

bool BlockingConnectSocket::writeable() {
  if (sendbuf.hasNext() || writer(sendbuf)) {
    stats.lastWrote = write(sock, &sendbuf.peek(), sendbuf.freespace());
    if (stats.lastWrote < 0) {
      dbg("write err: %d on %08X:%d", -stats.lastWrote, connectArgs.ipv4, connectArgs.port);
      disconnect(true);
      return false; // failure, lose data, drop connection and sleep until reconnected and writeInterest is called.
    } else if (stats.lastWrote > 0) {
      ++stats.writes; // logs attempts
      sendbuf.skip(stats.lastWrote);
      return sendbuf.hasNext(); // usually false
    } else {
      return true; // more please
    }
  } else {
    sendbuf.dump(); // in case writer() was sloppy.
    return false; // nothing to send, sleep until writeInterest is called.
  }
}

void BlockingConnectSocket::writeInterest() {
  source.writeInterest(sigc::hide_return(MyHandler(BlockingConnectSocket::writeable)));
}
