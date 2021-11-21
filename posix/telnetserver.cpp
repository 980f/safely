//"(C) Andrew L. Heilveil, 2017"
#include "telnetserver.h"

//called from socketeer accept
bool TelnetServer::enroll(int newfd, SockAddress &sadr){
  if(unsigned(newfd)<3){
    dbg("accept() gave us a console fd! : %u; We will not use that.",newfd);
//    return false;//absurd fd, caller will close it.
  }

  //purge dead ones, then try to add newone
  purge();
  while(clients.quantity()>=maxClients){
    clients.removeNth(0);
  }
  Socketeer &newby=*clients.append(factory(newfd,sadr));
  newby.setBlocking(false);
  return true;
}

Socketeer *TelnetServer::simpleFactory(int newfd, SockAddress &sadr){
  return new Socketeer(newfd,sadr);
}

TelnetServer::TelnetServer(Factory aFactory):
factory(nullptr,aFactory){
  //#nada
}

bool TelnetServer::isStarted(){
  return isConnected();
}

void TelnetServer::forEach(const TelnetServer::TelnetAction &pollone){
  for(ChainScanner<Socketeer> list(clients);list.hasNext();){
    Socketeer &console(list.next());
    pollone(console);
  }
}

bool TelnetServer::poll(const TelnetAction &pollone){
  if(!isStarted()){
    return false;
  }
  //check open connections
  forEach(pollone);
  //then check for a new one
  if(accept([this](int newfd,SockAddress &sadr){//if new connection
    return enroll(newfd,sadr);
   })){
    return true;
  }
  return false;
}

void TelnetServer::purge(){
  forEach([this](Socketeer &sock){
    if(sock.isDead()){
      clients.remove(&sock);
    }
  });
}

void TelnetServer::close(){
  forEach([](Socketeer &sock){sock.close();});//close clients
  Socketeer::close();//close server itself
  //don't need to purge, destructor will automatically do that.
}

Socketeer *TelnetServer::newest(){
  return clients.last();
}
