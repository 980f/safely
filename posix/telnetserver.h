#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include "socketeer.h"
#include "chain.h"
#include "hook.h"

/** derives from socket that is the listener. has list of active clients */
class TelnetServer: public Socketeer {
  Chain<Socketeer> clients;
  bool enroll(int newfd,SockAddress &sadr);

public:

  using Factory = Socketeer *(int newfd, SockAddress &sadr);
  Hooker<Socketeer *,int /*newfd*/, SockAddress &/*sadr*/> factory;

  static Socketeer *simpleFactory(int newfd,SockAddress &sadr);

  TelnetServer(Factory aFactory=simpleFactory);
//  void start();
  bool isStarted();

  using TelnetAction = std::function<void(Socketeer&)>;
  void forEach(const TelnetAction &pollone);
  /** check up on connection, accept clients */
  bool poll(const TelnetAction &pollone);
  /** delete dead clients */
  void purge();
  unsigned maxClients;

  Socketeer *newest();
};
#endif // TELNETSERVER_H
