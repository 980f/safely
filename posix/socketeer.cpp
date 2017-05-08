#include "socketeer.h"

#include <sys/socket.h>  //struct sockaddr, gethostbyname_r

#include "logger.h"
static Logger bug("SEER:");

int Socketeer::resolve(){
  char someinfo[100];//noone tells me how big this should be!
  struct hostent *result=&he;
  if(hostip!=0){
    bug("Already resolved to <%x>",hostip);
  }
  else {
    bug("Resolving Host named <%s>",hostname);
    int herrno;
    int error= gethostbyname_r (
      hostname, &he,
      someinfo, sizeof(someinfo)-1,
      &result, //pointer to pointer to same thing that 'he' is a pointer to.
      &herrno
    );
    if( error){
      bug("Resolve failed: %s",error);
      hostip=0;
      failure(errno);
      return error;
    }
    hostip=*(int *)he.h_addr_list[0];
  }
  host.sin_family = AF_INET;      // address family == internet
  host.sin_port = htons(port);    // u16, network byte order */
  host.sin_addr.s_addr = hostip;  // u32 network byte order
  EraseThing(host.sin_zero/*8*/); // zero the rest of the struct */
  return 0;
}

Socketeer& Socketeer::init(int hostip,int port){
  this->hostip=hostip;
  this->port=port;
  setBlocking(false);
  return *this;
}

Socketeer &Socketeer::init(const char *hostname, int port){
  this->hostname=hostname;
  this->port=port;
  setBlocking(false);
  return *this;
}


const char * Socketeer::setReadTimeout(int timeoutMillis){
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
  info.fd = fd;
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

//char Socketeer::read(){
//  if(isOpen()){
//    int revents;
//    if(okValue(revents,waitFor(POLLIN,readtoMillis))){
//      if(revents>0 && (revents & POLLIN)){
//        read
//        char onecharbuf;
//        int numread=read(fnum,&onecharbuf,1);
//        if(numread==1){
//          return onecharbuf;
//        }
//        lastErrno=errno;
//        } else {
//          lastErrno=revents;
//        }
//    }
//    return -1;///not a char
//}

const char *Socketeer::badError(const char *detail){
  bug(detail);
  close();
  return detail;
}

const char * Socketeer::Connect(int milliTimeout){
  if(isOpen()){
    return "already connected";
  }
  int fnum;
  if(okValue(fnum,socket(AF_INET, SOCK_STREAM, 0))){// af_inet==IPV4 ,sock_stream ==TCP
    preopened(fnum,true);
  } else {
    return "Couldn't make socket";
  }

  resolve();

  if(!setBlocking(false)){//so that we can time it out.
    return badError("couldn't clear blocking for connecting");
  }

  bug("Calling connect");
  if(failed( connect(fd, (struct sockaddr *)&host, sizeof(struct sockaddr)))){
    if (errornumber== EINPROGRESS) {
      errornumber=0;//the above code is proper for 'waiting'
      bug("Waiting up to %d millis for connection to complete.",milliTimeout);

      int pollresponse = waitFor(POLLIN | POLLOUT, milliTimeout);
      bug("Wait got: %04x",pollresponse);//give them the raw code

      errornumber=pollresponse;//just in case we forget
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
    } else {
      return badError("Couldn't connect");
    }
  }

  bug("Connected OK");

//todo:1 do we leave it nonblocking?
  errornumber=0;//ESUCCESS
  return nullptr;//success
}


///* blocking half duplex reader
//on error return negative of number of bytes read before error.
//0 bytes is an error in its own right.

//@todo: this seems useless, it might read past interesting char!
//must read a byte at a time.
//*/
//int Socketeer::ReceiveUntil(char *buf,int maxlen,int termchar){
//  int received;
//  int wad;

//  for(received=0;received<maxlen;){
//    wad=recv(fnum, &buf[received], maxlen-received, 0 /*flags*/);
//    if(wad>0){
//      received+=wad;
//      if(buf[received-1]==termchar){
//        return received; //success
//      }
//    } else {
//      lastErrno=errno;
//      break;
//    }
//  }
//  return -received; //error
//}

//int Socketeer::Send(const char *string){
//  if(fnum>0){
//    return send(fnum, string, strlen(string), 0);
//  } else {
//    return -1;
//  }
//}

//int Socketeer::Recv(char *buf,int maxlen){
//  if(isOpen()){
//    int any=waitFor(POLLIN,readtoMillis);
//    if(any>0 && (any&POLLIN)){
//      return recv(fnum, buf,maxlen , 0);//may not read all, but at least one byte should be there
//    }
//  }
//  return -1;
//}

Socketeer::Socketeer ():Fildes("SOCK"){
  hostname=0;
  hostip=0;
  port=0;
  isBlocking=false;
  readtoMillis=0;
}
