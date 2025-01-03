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
  server.sock.close();
  if(permanently){
    server.connectArgs.erase();
  }
}

void TcpServer::onAttach(int fd, uint32_t ipv4){
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

TcpServer::ServerSocket::ServerSocket(uint32_t remoteAddress, int port):
  TcpSocketBase(~0,remoteAddress,port){
  //#nada
}

bool TcpServer::ServerSocket::accept(int backlog, Spawner spawner){

  sock.preopened( ::socket(AF_INET, SOCK_STREAM, 0),true);
  if(sock.isOpen()){
    SocketAddress sad(connectArgs);
    //Setting the socket to reuse the address if we fail and restart.
    //This keeps us from getting a failed to bind error.
    sock.setOption(SOL_SOCKET, SO_REUSEADDR, int(1));

    //The sad.addr should be set to the wildcard 0.0.0.0, we don't care about the address yet.
    //when we do care which interface we listen on then figure out its address, don't trust eth naming unless we creat our own synonyms or pass that in as a configuration string.
    if(0 == ::bind(sock.asInt(), sad.addr(),sizeof(sad.sin))){
      //todo:1 the following references to errno make no sense, for instance they look for changes in errno instead of checking and storing it at the moment that it is valid.
      int listenRetval(0);
      int errorNum(errno);
      listenRetval = ::listen(sock,backlog);
      if (errorNum != errno){
        dbg("::listen failed with code %d errno: %d", listenRetval, errorNum);
      }
      this->spawner=spawner;
      //todo:00 sigc3 is killing me! source.listen( MyHandler(TcpServer::ServerSocket::incoming),bind((MyHandler(TcpServer::ServerSocket::disconnect),true));
      return true;
    } else {
      dbg("failed to bind %08X", connectArgs.ipv4);
      return false;
    }
  } else {
    dbg("failed to make server socket for %08X", connectArgs.ipv4);
    return false;
  }
}

void TcpServer::ServerSocket::incoming(){
  SocketAddress sad;  // :(
  //bi-directional arg, max length in, actual length out
  socklen_t length(sad.len());
  unsigned spawnedfd = ::accept(sock, sad.addr(), &length);
  if (spawnedfd != BadIndex) {
    spawner(spawnedfd,sad.hostAddress());//todo:0 redo to pass SocketAddress once we are sure we are dumping Gio::stuff.
  } else {
    dbg("ERROR on accept");
  }
}
