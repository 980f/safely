/* $Source: /cvs/pmclibs/socketeer.cpp,v $ */
/* $Revision: 1.7 $ */
#include "socketeer.h"

#include <errno.h>
#include <string.h>  //bzero, strlen

#include <sys/socket.h>  //struct sockaddr, gethostbyname_r
#include <unistd.h>  //read() close() other int fd type functions
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include "safestr.h" //nonTrivial
#include "bugger.h"
static Bugger bug("SEER:");


int Socketeer::resolve(){
  char someinfo[100];//noone tells me how big this should be!
  struct hostent *result=&he;
  if(hostip!=0){
    bug.println("Already resolved to <%x>",hostip);
  }
  else {
    bug.println("Resolving Host named <%s>",hostname);

    int error= gethostbyname_r (
      hostname, &he, 
      someinfo, sizeof(someinfo)-1,
      &result, //pointer to pointer to same thing that 'he' is a pointer to. 
      &lastErrno
    );
    if( error){
      bug.println("Resolve failed: %s",error);
      hostip=0;
      lastErrno=errno;
      return error;
    }  
    hostip=*(int *)he.h_addr_list[0];
  } 
  host.sin_family = AF_INET;      // address family == internet
  host.sin_port = htons(port);    // u16, network byte order */
  host.sin_addr.s_addr = hostip;  // u32 network byte order 
  bzero(&(host.sin_zero), 8);     // zero the rest of the struct */
}

Socketeer& Socketeer::Init(int hostip,int port){
  this->hostip=hostip;
  this->port=port;
  setBlocking(true);
  return *this;
}

Socketeer &Socketeer::Init(const char *hostname, int port){
  this->hostname=hostname;
  this->port=port;
  setBlocking(true);
  return *this;
}

const char * Socketeer::setBlocking(bool block){
  if(fnum<=0){
    return "must open before setting blocking mode";
  }
  int flags= fcntl (fnum, F_GETFL, 0);//get the socket's flags
  if (flags < 0){
    return "couldn't access flags";
  }
  if (fcntl (fnum, F_SETFL,  block? flags &~ O_NONBLOCK : flags | O_NONBLOCK) < 0){
    return "couldn't set flags";
  }
  isBlocking=block;
  return 0;
}

const char * Socketeer::setReadTimeout(int timeoutMillis){
  if(fnum<=0){
    return "must open before setting timeout";
  }
  if(timeoutMillis>0){
    if(isBlocking){
      setBlocking(false);
    }
    readtoMillis=timeoutMillis;
  }
  return 0;
}

#include <poll.h>
#define POLLTimeout 0x8000 

int Socketeer::waitFor(int events, int timout){
  pollfd info;
  info.fd = fnum;
  info.events = events;//POLLIN | POLLOUT /* superfluous | POLLERR | POLLHUP*/; //read ready, write ready, error, hangup.
  int rc=poll (&info, 1, timout); 
  if(rc>0){//==1 to be picky
    return info.revents;
  } 
  if(rc==0){//timeout
    return POLLTimeout;//lump in with other errors
  }
  return rc;
}

char Socketeer::read(){
  if(fnum>0){
    int revents=waitFor(POLLIN,readtoMillis);
    if(revents>0 && (revents & POLLIN)){
      char onecharbuf;
      int numread=::read(fnum,&onecharbuf,1);
      if(numread==1){
        return onecharbuf;
      } 
      lastErrno=errno;
    } else {
      lastErrno=revents;
    }
  } 
  return -1;///not a char
}

const char *Socketeer::badError(const char *detail){
  bug.println(detail);
  Release();
  return detail;
}

const char * Socketeer::Connect(int milliTimeout){
  if(fnum>0){
    return "already connected";
  }
  fnum=socket(AF_INET, SOCK_STREAM, 0);// af_inet==IPV4 ,sock_stream ==TCP
  if(fnum <=0) {
    lastErrno=errno;
    return "Couldn't make socket";  
  }

  resolve();
  
  const char *error=setBlocking(false);//so that we can time it out.
  if(error){
    return badError(error);
  }

  bug.println("Calling connect");
  if(connect(fnum, (struct sockaddr *)&host, sizeof(struct sockaddr))<0){
    lastErrno=errno;
    if (lastErrno != EINPROGRESS) {
      return badError("Couldn't connect");                  
    } else {
      lastErrno=0;//the above code is proper for 'waiting'
    }
  }
  
  bug.println("Waiting up to %d millis for connection to complete.",milliTimeout);
  
  int pollresponse = waitFor(POLLIN | POLLOUT, milliTimeout);
  bug.println("Wait got: %04x",pollresponse);//give them the raw code
  
  lastErrno=pollresponse;//just in case we forget
  //report just the most significant fault
  if(pollresponse<0){
    return badError("Socket error");
  }
  if (pollresponse & POLLTimeout){
    return badError("POLLTimeout");
  }
  if (pollresponse & POLLNVAL){
    return badError("POLLNVAL");
  }
  if (pollresponse & POLLERR){
    return badError("POLLERR");
  }
  if (pollresponse & POLLHUP){
    return badError("POLLHUP");
  }
  if (!(pollresponse & (POLLIN | POLLOUT))){
    return badError("Neither POLLIN or POLLOUT");
  }
  bug.println("Connected OK");

//do we leave it nonblocking?
  lastErrno=0;//ESUCCESS
  return 0;//success
}

Socketeer& Socketeer::Release(){
  if(fnum>0){
    close(fnum);
    fnum=0;
  }
  return *this;
}

/* blocking half duplex reader 
on error return negative of number of bytes read before error.
0 bytes is an error in its own right.

@todo: this seems useless, it might read past interesting char!
must read a byte at a time.
*/
int Socketeer::ReceiveUntil(char *buf,int maxlen,int termchar){
  int received;
  int wad;
  
  for(received=0;received<maxlen;){
    wad=recv(fnum, &buf[received], maxlen-received, 0 /*flags*/);
    if(wad>0){
      received+=wad;
      if(buf[received-1]==termchar){
        return received; //success
      }
    } else {
      lastErrno=errno;
      break;
    }
  }
  return -received; //error
}

int Socketeer::Send(const char *string){
  if(fnum>0){
    return send(fnum, string, strlen(string), 0);
  } else {
    return -1;
  }
}

int Socketeer::Recv(char *buf,int maxlen){
  if(fnum>0){
    int any=waitFor(POLLIN,readtoMillis);
    if(any>0 && (any&POLLIN)){
      return recv(fnum, buf,maxlen , 0);//may not read all, but at least one byte should be there
    }
  } 
  return -1;
}

Socketeer::Socketeer (){
  hostname=0;
  hostip=0; 
  port=0;
  fnum=0;
  isBlocking=true;
  readtoMillis=0;
  lastErrno=0;
  //host;
  //he;
}

/* $Id: socketeer.cpp,v 1.7 2004/03/26 17:43:36 andyh Exp $ */
