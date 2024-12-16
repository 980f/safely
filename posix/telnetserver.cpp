//"(C) Andrew L. Heilveil, 2017"
#include "telnetserver.h"

//called from socketeer accept
bool TelnetServer::enroll(int newfd, SockAddress &sadr) {
  if (unsigned(newfd) < 3) {
    dbg("accept() gave us a console fd! : %u; We will not use that.", newfd);
    //    return false;//absurd fd, caller will close it.
  }

  //purge dead ones, then try to add new one
  purge();
  while (clients.quantity() >= maxClients) {//if too many we boot out the oldest, not our problem if they don't like it!
    clients.removeNth(0);//we could scan for idle ones ... once we start recording transfer rates.
  }
  Socketeer &newby = *clients.append(factory(newfd, sadr));
  newby.setBlocking(false);
  return true;
}

Socketeer *TelnetServer::simpleFactory(int newfd, SockAddress &sadr) {
  return new Socketeer(newfd, sadr);
}

TelnetServer::TelnetServer(Factory aFactory): factory(nullptr, aFactory) {}

bool TelnetServer::isStarted() {
  return isConnected();
}

void TelnetServer::forEach(const TelnetServer::TelnetAction &pollone) {
  for (ChainScanner<Socketeer> list(clients); list.hasNext();) {
    Socketeer &console(list.next());
    pollone(console);
  }
}

bool TelnetServer::poll(const TelnetAction &pollone) {
  if (!isStarted()) {
    return false;
  }
  //check open connections
  forEach(pollone);
  //then check for a new one
  if (accept([this](int newfd, SockAddress &sadr) { //if new connection
      return enroll(newfd, sadr);
    })) {
    return true;
  }
  return false;
}

void TelnetServer::purge() {
  forEach([this](Socketeer &sock) {
    if (sock.isDead()) {
      clients.remove(&sock);
    }
  });
}

void TelnetServer::closeAll() {
  forEach([](Socketeer &sock) {
    sock.close();
  }); //close clients
  Socketeer::close(); //close server itself
  //don't need to purge, destructor will automatically do that.
}

Socketeer *TelnetServer::newest() {
  return clients.last();
}
