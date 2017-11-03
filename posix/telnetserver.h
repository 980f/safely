#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include "socketeer.h"
#include "chain.h"
#include "hook.h"

/** derives from socket that is the listener. has list of active clients */
class TelnetServer : public Socketeer {
  Chain<Socketeer> clients;
  bool enroll(int newfd,SockAddress &sadr);

public:
/** you get to pick a derived class of Socketeer for your client. */
  using Factory = Socketeer * (int newfd, SockAddress & sadr);
  Hooker<Socketeer *,int /*newfd*/, SockAddress & /*sadr*/> factory;
/** default factory is for the base class Socketeer */
  static Socketeer *simpleFactory(int newfd,SockAddress &sadr);

  /** the factory is usually set when the host is defined */
  TelnetServer(Factory aFactory = simpleFactory);
  void start();
  bool isStarted();

  using TelnetAction = std::function<void(Socketeer&)>;
  void forEach(const TelnetAction &pollone);
  /** check up on connection, accept clients */
  bool poll(const TelnetAction &pollone);
  /** delete dead clients */
  void purge();
  //only checked when clients connect
  unsigned maxClients;
  //not an override
  void close();
  Socketeer *newest();
}; // class TelnetServer
#endif // TELNETSERVER_H
