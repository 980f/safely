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
   * @param service  standard name or decimal string of port
   * if you know the name is numeric update the hints.ai_flags to reflect that to save time and resources.
*/
  bool get(const char *name, unsigned port=0,const char *service=nullptr);
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
  unsigned portnumber; //use when service is trivial.
  /***/
  HostInfo connectArgs;
  /** -1 not connected, 0 trying, 1 am connected */
  int connected;
private:
  bool makeSocket();
  bool resolve();//convert hostname into ip address.
  const char *badError(const char *detail);

public:
  Socketeer ();
  /** build one around a client that a server has accepted */
  Socketeer (int newfd,SockAddress &sadr);
  /** record connection parameters but don't do anything with them. @returns this */
  Socketeer &init(TextKey hostname, TextKey service);
//  /** record connection parameters but don't do anything with them. @returns this */
//  Socketeer &init(unsigned hostip, unsigned port);

  bool isConnected(){
    return connected>0;
  }

  /** connect using stored host parameters, @returns nullptr on success else it is an error message. */
  const char * connect();//returns error message or null string pointer

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

  using Spawner = std::function<void (int /*fd*/, SockAddress &/*sadr*/)>;
  bool accept(const Spawner &spawner,bool blocking=false);

};

#endif
