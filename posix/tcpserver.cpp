#include "tcpserver.h"
#include "logger.h"

////////////////////////////

TcpServer::TcpServer(int port, int interfaceIP, int backlog):
  server(interfaceIP,port),
  backlog(backlog),
  connects(0),
  disconnects(0),
  listening(false){
  //#nada
}

TcpServer::TcpServer():
  server(0,0),
  backlog(0),
  connects(0),
  disconnects(0),
  listening(false){
  //#nada
}

TcpServer::~TcpServer(){
  shutdown(true);
}

bool TcpServer::startup(){
  if(listening){
    //todo:2 dbg() if already started?
    return true;
  }
  if(server.connectArgs.isPossible()){//precheck for debug

    return server.accept(backlog,MyHandler(TcpServer::onAttach));

  } else {
    return false;
  }

}

bool TcpServer::serveAt(int port, int interfaceIP, int backlog){
  server.connectArgs.port=port;
  server.connectArgs.ipv4=interfaceIP;
  this->backlog=backlog;
  return startup();
}

void TcpServer::shutdown(bool permanently){
  //  listenerp->close();
  server.fd.close();
  if(permanently){
    server.connectArgs.erase();
  }
}

void TcpServer::onAttach(int fd, u32 ipv4){
  if(TcpSocket *spawned=spawnClient(fd,ipv4)){
    ++connects;
    dbg("Now serving %08X",spawned->connectArgs.ipv4);
  } else {
    dbg("Issuing a \"rejected by foreign host\" ");
  }
}


bool TcpServer::isConnected(){
  return server.isConnected();
}


TcpServer::ServerSocket::ServerSocket(u32 remoteAddress, int port):
  TcpSocketBase(~0,remoteAddress,port)
{
}

bool TcpServer::ServerSocket::accept(int backlog, Spawner spawner){

  fd.preopened( ::socket(AF_INET, SOCK_STREAM, 0));
  if(fd.isOpen()){
    SocketAddress sad(connectArgs);
    const int optval(1);
    //Setting the socket to reuse the address if we fail and restart.
    //This keeps us from getting a failed to bind error.
    ::setsockopt(fd,SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    //Force the socket to be on specific interface, instead of an address.
    //This may become a problem if we want to serve a socket on eth1 but can easily be fixed.
    //::setsockopt(sock.fd,SOL_SOCKET, SO_BINDTODEVICE, &ETH0, sizeof(ETH0));

    //The sad.addr should be set to the wildcard 0.0.0.0, we don't care about the address yet.
    if(0 == ::bind(fd, sad.addr(),sizeof(sad.sin))){
      int listenRetval(0);
      int errorNum(errno);
      listenRetval = ::listen(fd,backlog);
      if (errorNum != errno){
        dbg("::listen failed with code %d errno: %d", listenRetval, errorNum);
      }
      this->spawner=spawner;
//      source.incoming=sock.input(MyHandler(TcpServer::ServerSocket::incoming));
//      source.hangup=sock.hangup(MyHandler(TcpServer::ServerSocket::disconnect));
      return true;
    } else {
      dbg("failed to bind %08X", connectArgs.ipv4);
      return false;
    }
  }
  dbg("failed to make server socket for %08X", connectArgs.ipv4);
  return false;
}

bool TcpServer::ServerSocket::incoming(){
  SocketAddress sad;  // :(
  //bi-directional arg, max length in, actual length out
  socklen_t length(sad.len());
  unsigned spawnedfd = ::accept(fd, sad.addr(), &length);
  if (spawnedfd != BadIndex) {
    spawner(spawnedfd,sad.hostAddress());//todo:0 redo to pass SocketAddress once we are sure we are dumping Gio::stuff.
  } else {
    dbg("ERROR on accept");
  }
  return true;
}
