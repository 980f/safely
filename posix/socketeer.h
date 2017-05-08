#ifndef socketeer_h
#define socketeer_h

#include "fildes.h"
#include <netdb.h>       //struct hostent

/** this started life as a blocking socket, now it is an extension to the Fildes class that knows about the special opening of sockets,
and has the dregs of blocking I/O in comments.*/
class Socketeer: public Fildes {
private:
  struct hostent he;//used by resolve, kept for debug

  const char *hostname;
  int hostip;
  int port;


  struct sockaddr_in host; //in.h
  bool isBlocking;
  int readtoMillis;
private:

  int resolve();//convert hostname into ip address.
  const char *badError(const char *detail);

  int waitFor(int events, int timout);
  /* termchar is left at end of buffer and room for it must be included in 'maxlen' allocation*/
//  int ReceiveUntil(char *buf,int maxlen,int termchar);//not working yet

public:
  Socketeer ();
  Socketeer &init(const char *hostname, int port);
  Socketeer &init(int hostip, int port);

  const char * setReadTimeout(int timeoutMillis);
  const char * Connect(int milliTimeout=0);//returns error message or null string pointer


  /** blocking read of a byte, using poll() to wait.*/
//  char readBlocking();
//  int Send(const char *string);
//  int Recv(char *buf,int maxlen);

} ;

#endif
