#ifndef socketeer_h
#define socketeer_h

#include "fildes.h"
#include "textpointer.h" //for textual hostname
#include <functional>
#include <netdb.h>       //addrinfo

/** union of socket addresses, which are distinguished primarily by their length */
struct SockAddress {
  sockaddr address;
  socklen_t length=sizeof(address);
  unsigned getPort();
  unsigned getIpv4();
};

/** wrapper to make sure we free any we fetch*/
class HostInfo {
  bool gotten;
  int getError;
public: //
  int lastErrno;//like posixwrapper, migth refactor to use it.
public:
  addrinfo *res;
  addrinfo hints;

  /** the only part of addrinfo that we need to hold on to */
  SockAddress connected;

  HostInfo();
  /** set hints, ie make given entity suitable for use in the hints param of getaddrinfo() */
  void hint(bool tcp=true);
  /** @param node  name of host, null for localhost (serving, in which case also set hints AI_PASSIVE)
   * @param service  standard name or decimal string of port or nullptr in which case set @param port to something interesting.
   * if you know the name is numeric update the hints.ai_flags to reflect that to save time and resources.
*/
  bool lookup(const char *name, unsigned port=0,const char *service=nullptr);
  void keep(addrinfo *one=nullptr);
  const char *errorText(const char *ifOk="OK");
  void free();
  /** free all but first */
  void clip();
  ~HostInfo();
  //flags for hints: AI_NUMERICHOST, AI_PASSIVE&& null node for server.
};


/** this started life as a blocking socket, now it is an extension to the Fildes class that knows about the special opening of sockets */
class Socketeer: public Fildes {
private:
  Text hostname;//name as either a URL name, or dotted decimal IPV4 or hex:IPV6
  Text service;//name from /etc/services file or decimal image of port to connect to/serve.
  unsigned portnumber=0; //use when service is trivial.
  /***/
  HostInfo connectArgs;
  /** -1 not connected, 0 trying, 1 am connected */
  int connected=-1;
private:
  bool makeSocket();
  bool resolve();//convert hostname into ip address.
  const char *badError(const char *detail);

public:
  Socketeer ();
  /** build one around a client that a server has accepted */
  Socketeer (int newfd,SockAddress &sadr);
  /** record connection parameters but don't do anything with them. @returns this */
  Socketeer &init(TextKey hostname, unsigned portnum, TextKey service);
//  /** record connection parameters but don't do anything with them. @returns this */
//  Socketeer &init(unsigned hostip, unsigned port);

  bool isConnected();

  bool isDead();

  /** connect using stored host parameters, @returns nullptr on success else it is an error message. */
  const char * connect();//returns error message or null string pointer

  void disconnect(); //forcefully close, release resources
  /** note that some socket options are settable in the socket() call.
*/
  template <typename ArgType> bool setSocketOption(unsigned optionEnum, ArgType optval,unsigned level=SOL_SOCKET){
    return ok(::setsockopt(fd,level, optionEnum, &optval, sizeof(ArgType)));
  }

  template <typename ArgType> bool getSocketOption(unsigned optionEnum, ArgType &optval,unsigned level=SOL_SOCKET){
    socklen_t len=sizeof(ArgType);
    return ok(::getsockopt(fd,level, optionEnum, optval, &len));
  }

  /** tcp doesn't have a flush command, the best we can do is suck out the data as fast as we can */
  void flush();

  /** starts listening, you will have to poll for accepts*/
  bool serve(unsigned backlog=1);

  using Spawner = std::function<bool (int /*fd*/, SockAddress &/*sadr*/)>;
  /** @returns whether a new connection was spawned. That requires that there be one pending and the spawner agent agrees to connect it.
   * @param blocking determines the blocking state of any newly spawned client connection. */
  bool accept(const Spawner &spawner,bool blocking=false);

  unsigned atPort(){
    return isConnected()? this->portnumber:BadIndex;
  }
//maydo: but so far we don't have a need for 'flags'
//  bool receive(Indexer<u8> &p,int flags){
//    recv(fd,&p.peek(),p.freespace(),flags);
//  }
};

#endif