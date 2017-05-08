#include "socketeer.h"

#include <sys/socket.h>  //struct sockaddr, gethostbyname_r

#include "logger.h"
#include "cheaptricks.h"
#include "string.h" //errno, we should make our own header for that!

#include <poll.h>  //for blocking connect.
#define POLLTimeout 0x8000


static Logger bug("SEER:");

bool Socketeer::makeSocket(){
  if(!resolve()){
    bug("Resolve error:\"%s\"",connectArgs.errorText(""));
    errornumber=connectArgs.lastErrno;
    return false;//couldn't resolve host
  }

  int fnum;
  if(okValue(fnum,::socket(connectArgs.res->ai_family, connectArgs.res->ai_socktype |SOCK_NONBLOCK, connectArgs.res->ai_protocol))){
    return preopened(fnum,true);
  } else {
    return false;
  }
}

bool Socketeer::resolve(){
//hint: AI_NUMERICSERV if hostname is a stringified numerical address.
  return connectArgs.get(hostname.c_str(),portnumber,service.nullIfEmpty());
}


Socketeer &Socketeer::init(TextKey hostname, TextKey service){
  this->hostname=hostname;
  this->service=service;
  return *this;
}

const char *Socketeer::badError(const char *detail){
  bug(detail);
  close();
  return detail;
}


const char * Socketeer::connect(){
  if(connected>0){
    return "already connected";
  }

  if(connected<0){
    if(makeSocket()){
      connected=0;
    } else {
      return "Couldn't make socket";
    }
  }
  //todo:1 iterate over connectArgs and try until one works.
  addrinfo *one=connectArgs.res;
  if(failed( ::connect(fd, one->ai_addr, one->ai_addrlen))){
    if (errornumber== EINPROGRESS || errornumber==EALREADY) {
      errornumber=0;//the above codes are 'still working on it'
      return nullptr;
    } else {
      //todo:1 do we release socket and go to connected=-1?
      return errorText();
    }
  } else {
    connected=1;
    connectArgs.keep(one);
    //maydo: connectArgs.free();
//maudo:    connectArgs.clip();//todo: integrate this with keep, i.e. keep (what we need from) the given one and free the rest.
    bug("Connected OK");
    errornumber=0;//forget any lingering error indication.
    return nullptr;//success
  }
}

void Socketeer::flush(){
  u8 bytes[4096];
  ByteScanner toilet(bytes,sizeof(bytes));
//can we stat a socket fd?
  unsigned notforever=10000;
  while(notforever-- && read(toilet)==sizeof(bytes)){
    //#nada
  }
}

bool Socketeer::serve(unsigned backlog){
  if(makeSocket()){

    if(!setSocketOption(SO_REUSEADDR, 1)){
      bug("Couldn't set REUSEADDR, proceeding anyway");
    }

    if(ok(bind(fd, connectArgs.res->ai_addr,connectArgs.res->ai_addrlen))){
      if(ok(listen(fd,backlog))){
        return true;
      } else {
        bug("Couldn't listen");
      }
    } else {
      bug("Couldn't bind");
    }
  } else {
    bug("Couldn't make socket");
  }
  return false;
}

bool Socketeer::accept(const Spawner &spawner, bool blocking){
  SockAddress sadr;
  int newfd=accept4(fd, &sadr.address, &sadr.length, blocking?0:SOCK_NONBLOCK);
  if(newfd!=BADFD){
    //we have a new socket!
    //and its address!
    spawner(newfd,sadr);
    return true;
  } else {
    return false;
  }
}

Socketeer::Socketeer ():Fildes("SOCK"),
  portnumber(BadIndex),
  connected(-1)
{
  //#nada
}

Socketeer::Socketeer(int newfd, SockAddress &sadr):Fildes("ClientSocket"){
  preopened(newfd,true);
//save other stuff for debug:
  connectArgs.connected=sadr;
}

//int Socketeer::waitFor(int events, int timout){
//  pollfd info;
//  info.fd = fd;
//  info.events = events;//POLLIN | POLLOUT /* superfluous | POLLERR | POLLHUP*/; //read ready, write ready, error, hangup.
//  int rc=poll (&info, 1, timout);
//  if(rc>0){//==1 to be picky
//    return info.revents;
//  }
//  if(rc==0){//timeout
//    return POLLTimeout;//lump in with other errors
//  }
//  return rc;
//}

///////////////////////////////

bool HostInfo::get(const char *name, unsigned port, const char *service){
  getError=getaddrinfo(name, service,&hints,&res);
  gotten=res!=nullptr;
  lastErrno= getError?errno:0;
  if(getError){
    return false;
  } else {
    if(service==nullptr){
      if(port){
        //inject port number into addr structs in the list.
        //that involves knowing what kind of address each is.
        //for now they had all better by inet addrs:
        for(addrinfo*ai=res;ai;ai=ai->ai_next){
          //todo: switch on res->ai_family
          sockaddr_in &host( *reinterpret_cast<sockaddr_in *>( ai->ai_addr));
          host.sin_port=port;
        }
      }
    }
    return true;
  }
}

void HostInfo::keep(addrinfo *one){
  if(one==nullptr){
    one=res;
  }
  if(one==nullptr){
    return;
  }
  memmove(&connected.address,one->ai_addr,connected.length=one->ai_addrlen);
}

const char *HostInfo::errorText(const char *ifOk){
  if(getError){
    if(getError==EAI_SYSTEM){
      return strerror(lastErrno);
    }
    return gai_strerror(getError);
  } else {
    return ifOk;
  }
}

void HostInfo::free(){
  if(flagged(gotten)){
    freeaddrinfo(res);
  } else {
    delete res;//pathological
  }
  res=nullptr;//improve detection of use after free
}

void HostInfo::clip(){
//todo: detect which member we use, free the others and set res to it. Until then we use the first and ditch the rest.
  if(gotten && res && res->ai_next){
    freeaddrinfo(res->ai_next);
  }
}

HostInfo::HostInfo():
  gotten(false),
  getError(0),
  res(nullptr)
{
  hint();
}

HostInfo::~HostInfo(){
  free();
}

void HostInfo::hint(bool tcp){
  hints.ai_family= AF_UNSPEC ;//   AF_INET and AF_INET6.
  hints.ai_socktype=tcp?SOCK_STREAM:SOCK_DGRAM;// SOCK_STREAM , someday: SOCK_DGRAM, 0 for any
  hints.ai_protocol =tcp?6:17;  //6 to limit to TCP, 17 for UDP else see https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
  hints.ai_flags =0; //todo: systematize for numeric name

  //the rest are zeroed for safety.
  hints.ai_addrlen=0;
  hints.ai_addr=nullptr;
  hints.ai_canonname=nullptr;
  hints.ai_next=nullptr;
}
