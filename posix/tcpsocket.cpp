#include "tcpsocket.h"
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include "logger.h"
#include <errno.h>
#include "netinet/tcp.h"
#include "microseconds.h"


TcpSocketBase::TcpSocketBase(int fd, u32 remoteAddress, int port):
  socketStats(),
  connectArgs(remoteAddress,port),
  fd("SOCKET")
{
  this->fd.preopened(fd);
}

TcpSocketBase::~TcpSocketBase()
{
}

bool TcpSocketBase::isConnected() const {
  return fd.isOpen();
}

TcpSocket::TcpSocket(int fd, u32 remoteAddress, int port):
  TcpSocketBase(fd,remoteAddress,port),
  autoConnect(false),
  sendbuf(){
  if(this->fd.isOpen()){
    startReception();
  }
}

u32 TcpSocketBase::remoteIpv4(){
  return connectArgs.ipv4;
}

bool TcpSocketBase::disconnect(){
  if(isConnected()){//#checking for debug purposes
    fd.close();
  }
  return false;
}

//////////////////////////////

bool TcpSocket::connect(unsigned ipv4, unsigned port){
  connectArgs.ipv4=ipv4;
  connectArgs.port=port;
  autoConnect=connectArgs.isPossible();
  return autoConnect&&reconnect();
}

void TcpSocket::startReception(){
//  source.incoming= sock.input(MyHandler(TcpSocket::readable));
//  source.hangup= sock.hangup(MyHandler(TcpSocket::hangup));
}

bool TcpSocket::reconnect(){
  bool wasConnected=isConnected();
  if(wasConnected) {
    disconnect(false);//don't notify since notify is likely to call connect and hence infinite loop.
  }
  fd.preopened(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
  if(fd.isOpen()) {
    int optval(1);
    //Setting the socket to reuse the address if we fail and restart
    ::setsockopt(fd,SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(wasConnected){
      ++socketStats.disconnects;//failure to make socket, so number in diags grows if problem persists.
      notifyConnected(false);
    }
  } else {
    return false;
  }

  //#setting the following should probaby be optional.
//  if(sock.setTCPopt(TCP_NODELAY, 1)){
//    dbg("Set TCP no delay returned %d",errno);
//  }

  //todo:1 research whether the default so_linger is false, we'd like a reset() rather than a gentle close() when we disconnect.
  SocketAddress sad(connectArgs);
  if(sad.connect(fd)) {
    if(EINPROGRESS != errno) {//'already connecting' is not an error (which may be a bad idea)
      disconnect(true);
      return false;
    }
  }
  ++socketStats.connects;

  startReception();
  notifyConnected(true);
  return true;
}

void TcpSocket::disconnect(bool andNotify){
  TcpSocketBase::disconnect();
  if(andNotify){
    ++socketStats.disconnects;
    notifyConnected(false);
  }
}


void TcpSocket::flush(){
  //there is no flush in TCP.
  u8 bytes[4096] = {0};
  //can we stat a socket fd?
  int notforever=10000;
  while(fd.read(bytes, sizeof(bytes))==sizeof(bytes)){
    //a debug message here might slow us down enough to never catch up with the source and hence make this an infinite loop.
    if(--notforever<=0){
      return;
    }
  }
}


bool TcpSocket::readable() {
  u8 bytes[4096] = {0};

  fd.read(bytes, sizeof(bytes));
  if(fd.lastRead < 0) {
//    dbg("read err: %d on %08X:%d",-stats.lastRead,connectArgs.ipv4, connectArgs.port); //useful for debug, but it will spam you
    disconnect(true);
    return false;
  } else if (fd.lastRead == 0) {//read returns zero when a client cleanly disconnects
    dbg("The remote client has disconnected");
    disconnect(true);
    return false;
  }
  if(fd.lastRead > 0) {//skip 0 to get better stats. NO! zero is returned for a closed connection
    ByteScanner chunk(bytes,fd.lastRead);
    ++socketStats.reads+=1;//todo:was a bug or just a confusing way to add 2
    reader(chunk);
  }
  return true;
}

/** argument ignored as it is always the one value we allow it to be in the create() call.*/
bool TcpSocket::writeable() {
  if(sendbuf.hasNext() || writer(sendbuf)){
   fd.lastWrote = write(fd, &sendbuf.peek(), sendbuf.freespace());
    if(fd.lastWrote<0) {
      dbg("write err: %d on %08X:%d", -fd.lastWrote,connectArgs.ipv4, connectArgs.port);
      disconnect(true);
      return false; //failure, lose data, drop connection and sleep until reconnected and writeInterest is called.
    } else if(fd.lastWrote>0) {
      ++socketStats.writes;//logs attempts
      sendbuf.skip(fd.lastWrote);
      return sendbuf.hasNext();//usually false
    } else {
      return true; //more please
    }
  } else {
    sendbuf.dump();//in case writer() was sloppy.
    return false;//nothing to send, sleep until writeInterest is called.
  }
}

/** expect this when far end of socket spontaneously closes*/
bool TcpSocket::hangup(){
  disconnect(true);
  return true;//todo:0 this probably should be false, may be moot.
}

//void TcpSocket::writeInterest() {
//  source.writeInterest(MyHandler(TcpSocket::writeable));
//}

TcpSocket::~TcpSocket(){
  disconnect(false);
}

sigc::connection TcpSocket::whenConnectionChanges(const BooleanSlot &nowConnected,bool kickme){
  if(kickme){
    nowConnected(isConnected());
  }
  return notifyConnected.connect(nowConnected);
}
///////////////////////////
TcpSocketBase::Stats::Stats(){
  clear();
}

void TcpSocketBase::Stats::clear(){
  connects=0;
  disconnects=0;
//  fd.lastRead=0;
//  fd.lastWrote=0;
  reads=0;
  writes=0;
}

///////////////////////////
bool TcpSocket::ConnectArgs::isPossible(){
  return port || ipv4;
}

TcpSocket::ConnectArgs::ConnectArgs(int ipv4, int port):
  ipv4(ipv4),
  port(port)
{
}

void TcpSocket::ConnectArgs::erase(){
  ipv4=0;
  port=0;
}

//////////////////////////////
SocketAddress::SocketAddress(){
  EraseThing(sin);
}

SocketAddress::SocketAddress(TcpSocket::ConnectArgs &cargs){
  EraseThing(sin);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(cargs.ipv4);
  sin.sin_port = htons(cargs.port);
}

u16 SocketAddress::hostPort() const {
  return ntohs(sin.sin_port);
}

u32 SocketAddress::hostAddress() const {
  return ntohl(sin.sin_addr.s_addr);
}

const sockaddr *SocketAddress::addr() const {
  return reinterpret_cast<const sockaddr*>(&sin);
}

sockaddr *SocketAddress::addr() {
  return reinterpret_cast<sockaddr*>(&sin);
}

int SocketAddress::len() const {
  return sizeof(sin);
}

bool SocketAddress::connect(int fd){
  // return (-1 == ::connect(...))
  return Fildes::BADFD==::connect(fd, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
}


BlockingConnectSocket::BlockingConnectSocket(int fd, u32 remoteAddress, int port) :
  TcpSocketBase(fd,remoteAddress,port),
  autoConnect(false),
  sendbuf(){
  if(this->fd.isOpen()){
    startReception();
  }
}

BlockingConnectSocket::~BlockingConnectSocket(){
  disconnect(false);
}

void BlockingConnectSocket::startReception() {
//    source.incoming= sock.input(MyHandler(BlockingConnectSocket::readable));
//    source.hangup= sock.hangup(MyHandler(BlockingConnectSocket::hangup));
}

void BlockingConnectSocket::disconnect(bool notify) {
  TcpSocketBase::disconnect();
  if (notify) {
    ++socketStats.disconnects;
    notifyConnected(false);
  }
}

bool BlockingConnectSocket::hangup() {
  disconnect(true);
  return true;
}

bool BlockingConnectSocket::readable() {
  u8 bytes[4096] = {0};

  fd.read(bytes, sizeof(bytes));
  if (fd.lastRead < 0) {
//    dbg("read err: %d on %08X:%d",-stats.lastRead,connectArgs.ipv4, connectArgs.port); //useful for debug, but it will spam you
    disconnect(true);
    return false;
  }
  if (fd.lastRead == 0) {//read returns zero when a client cleanly disconnects
    dbg("The remote client has disconnected");
    disconnect(true);
    return false;
  }
  // last read > 0
  //if(socketStats.lastRead > 0) {//skip 0 to get better stats. NO! zero is returned for a closed connection
    ByteScanner chunk(bytes,fd.lastRead);
    ++socketStats.reads+=1;
    reader(chunk);
  //}
  return true;
}

bool BlockingConnectSocket::connect(unsigned ipv4, unsigned port){
  connectArgs.ipv4=ipv4;
  connectArgs.port=port;
  autoConnect=connectArgs.isPossible();
  return autoConnect&&reconnect();
}

// This method will emulate a "timeout" on the connect
bool BlockingConnectSocket::reconnect(){
  bool wasConnected=isConnected();
  if(wasConnected) {
    disconnect(false);//don't notify since notify is likely to call connect and hence infinite loop.
  }
  fd.preopened( ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));

  if(!fd.isOpen()) {
    return false;
  }
  int optval(1);

  //Setting the socket to reuse the address if we fail and restart
  ::setsockopt(fd,SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  if(wasConnected){
    ++socketStats.disconnects;//failure to make socket, so number in diags grows if problem persists.
    notifyConnected(false);
  }
  //#setting the following should probaby be optional.
  //  if(sock.setTCPopt(TCP_NODELAY, 1)){
  //    dbg("Set TCP no delay returned %d",errno);
  //  }

  //todo:1 research whether the default so_linger is false, we'd like a reset()
  // rather than a gentle close() when we disconnect.
  SocketAddress sad(connectArgs);
  if(sad.connect(fd)) {
    // we did not immediately connect!  now emulate blocking socket with timeout!
    if (EINPROGRESS == errno) {
      socklen_t lon;
      MicroSeconds tv;
      int valopt;
      fd_set myset;
      int result;
      while (1) {
        tv=3;
        FD_ZERO(&myset);
        FD_SET(fd, &myset);
        result = select(fd+1, NULL, &myset, NULL, &tv);
        if (result < 0 && errno != EINTR) {
          disconnect(true);
          return false;
        }
        else if (result > 0) {
          lon = sizeof(int);
          if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
            disconnect(true);
            return false;
          }
          // Check the value returned...
          if (valopt) {
            disconnect(true);
            return false;
          }
          break;
        }
        else {
          fprintf(stderr, "Timeout in select() - Cancelling!\n");
          disconnect(true);
          return false;
        }
      }
    }
    else { // not a "EINPROGRESS"
      disconnect(true);
      return false;
    }
  }

  ++socketStats.connects;
  startReception();
  notifyConnected(true);
  return true;
}

bool BlockingConnectSocket::writeable() {
  if (sendbuf.hasNext() || writer(sendbuf)){
    fd.write( &sendbuf.peek(), sendbuf.freespace());
    if(fd.lastWrote<0) {
      dbg("write err: %d on %08X:%d", -fd.lastWrote,connectArgs.ipv4, connectArgs.port);
      disconnect(true);
      return false; //failure, lose data, drop connection and sleep until reconnected and writeInterest is called.
    } else if(fd.lastWrote>0) {
      ++socketStats.writes;//logs attempts
      sendbuf.skip(fd.lastWrote);
      return sendbuf.hasNext();//usually false
    } else {
      return true; //more please
    }
  } else {
    sendbuf.dump();//in case writer() was sloppy.
    return false;//nothing to send, sleep until writeInterest is called.
  }
}

//void BlockingConnectSocket::writeInterest() {
//  source.writeInterest(MyHandler(BlockingConnectSocket::writeable));
//}


