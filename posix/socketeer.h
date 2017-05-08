#ifndef socketeer_h
#define socketeer_h


//#include "sys/socket.h"
//#include "netinet/in.h"
#include <netdb.h>       //struct hostent

class Socketeer {
private:
  struct hostent he;//used by resolve, kept for debug

  const char *hostname;
  int hostip; 
  int port;
  
  int fnum;
  struct sockaddr_in host; //in.h
  bool isBlocking;
  int readtoMillis;
private:
  int resolve();//convert hostname into ip address.
  const char *badError(const char *detail);
  int waitFor(int events, int timout);
  const char * setBlocking(bool block);
  int ReceiveUntil(char *buf,int maxlen,int termchar);//not working yet
  
public:  
  Socketeer ();
  Socketeer &Init(const char *hostname, int port);
  Socketeer &Init(int hostip, int port);
  
  char read();//read one byte
  const char * setReadTimeout(int timeoutMillis);
  const char * Connect(int milliTimeout=0);//returns error message or null string pointer
  
  Socketeer &Release();//close() and so on.
  /*termchar is left at end of buffer and room for it must be included in 'maxlen' allocation*/
  int Send(const char *string);
  int Recv(char *buf,int maxlen);
  int lastErrno;
} ;


#endif

