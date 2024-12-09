//"(C) Andrew L. Heilveil, 2017"
#include "socketeer.h"

#include <charscanner.h>
#include <sys/socket.h>  //struct sockaddr, gethostbyname_r

#include "fcntlflags.h" //to open control port

#include "logger.h"
#include "cheaptricks.h"
#include "string.h" //errno, we should make our own header for that!

#include <poll.h>  //for blocking connect.
#define POLLTimeout 0x8000


static Logger bug("SEER");

bool Socketeer::makeSocket(){
  if(!resolve()) {
    bug("Resolve error:\"%s\"",connectArgs.errorText(""));
    errornumber = connectArgs.lastErrno;
    return false;//couldn't resolve host
  }

  int fnum = ~0;
  if(okValue(fnum,::socket(connectArgs.res->ai_family, connectArgs.res->ai_socktype | SOCK_NONBLOCK, connectArgs.res->ai_protocol))) {
    return preopened(fnum,true);
  } else {
    return false;
  }
} // Socketeer::makeSocket

bool Socketeer::resolve(){
//hint: AI_NUMERICSERV if hostname is a stringified numerical address.
  return connectArgs.lookup(hostname.c_str(),portnumber,service.nullIfEmpty());
}

Socketeer &Socketeer::init(TextKey hostname, unsigned portnum,TextKey service){
  this->hostname = hostname;
  this->portnumber = portnum;
  this->service = service;
  return *this;
}

bool Socketeer::isConnected(){
  return connected>0;
}

bool Socketeer::isDead(){
  //todo:1 how do we detect a client is dead? Probably by last read error.
  if(lastRead==EPIPE) {
    disconnect();
    lastRead = 0;
  }
  return connected<0;
}

bool Socketeer::isBlocking(){
  bool nonblocking = false;
  if(getSingleFlag(O_NONBLOCK,nonblocking)) {
    return !nonblocking;
  } else {
    return true;//in absence of sure knowledge return pessimistic answer
  }
}

const char *Socketeer::badError(const char *detail){
  bug(detail);
  close();
  return detail;
}

const char * Socketeer::connect(){
  if(connected>0) {
    return "already connected";
  }

  if(connected<0) {
    if(makeSocket()) {
      connected = 0;
    } else {
      return "Couldn't make socket";
    }
  }
  //todo:1 iterate over connectArgs and try until one works.
  addrinfo *one = connectArgs.bestAddress();
  if(failed( ::connect(fd, one->ai_addr, one->ai_addrlen))) {
    if (errornumber== EINPROGRESS || errornumber==EALREADY) {
      errornumber = 0;//the above codes are 'still working on it'
      return nullptr;
    } else {
      //todo:1 do we release socket and go to connected=-1?
      return errorText();
    }
  } else {
    connected = 1;
    connectArgs.keep(one);
    //maydo: free the unused ones here, instead of later.
    bug("Connected OK");
    errornumber = 0;//forget any lingering error indication.
    return nullptr;//success
  }
} // Socketeer::connect

void Socketeer::disconnect(){
  switch(connected) {
  case -1: //already dead
    return;
  case 0: //while connecting
    connected = -1;
    return;
  case 1: //
    close();
    connected = -1;
    return;
  } // switch
} // Socketeer::disconnect

void Socketeer::flush(){
  u8 bytes[4096];
  ByteScanner toilet(bytes,sizeof(bytes));
//can we stat a socket fd?
  for(unsigned notforever = 10000;notforever-- && read(toilet)&&toilet.freespace()==0;) {
    toilet.rewind();//was defective until this was added, only flushed at most 4k bytes!
  }
}

bool Socketeer::serve(unsigned backlog){
  if(connected>0) {
    return true;//"already serving";
  }

  if(connected<0) {
    if(makeSocket()) {
      connected = 0;

      if(!setSocketOption(SO_REUSEADDR, 1)) {
        bug("Couldn't set REUSEADDR, proceeding anyway");
      }

      if(ok(bind(fd, connectArgs.res->ai_addr,connectArgs.res->ai_addrlen))) {
        if(ok(listen(fd,backlog))) {
          connected = 1;
          return true;
        } else {
          //does 'unbinding' make sense?
          bug("Couldn't listen");
        }
      } else {
        bug("Couldn't bind");
      }
    } else {
      bug("Couldn't make socket");
    }
  }
  return false;
} // Socketeer::serve

bool Socketeer::accept(const Spawner &spawner, bool blocking){
  if(connected>0) {
    SockAddress sadr;
    int flags = blocking ? 0 : SOCK_NONBLOCK;
    int newfd = BADFD;
    if(okValue(newfd,accept4(fd, &sadr.address, &sadr.length, flags))) {
      //we have a new socket! //and its address!
      bug("accepted: %08x on port %u as fd: %d",sadr.getIpv4(),sadr.getPort(),newfd);
      if(spawner(newfd,sadr)) {
        return true;
      } else {//it was rejected by external rule
        Fildes::Close(newfd);
        return false;//service rejected the connection
      }
    } else {
      return false;//no connection (we are polling so this is normal)
    }
  } else {
    return false;//not listening.
  }
} // Socketeer::accept

unsigned Socketeer::atPort(){
  return isConnected() ? portnumber : BadIndex;
}

Socketeer::Socketeer() : Fildes("SOCK"),
  portnumber(BadIndex),
  connected(-1){
  //#nada
}

Socketeer::Socketeer(int newfd, SockAddress &sadr) : Fildes("ClientSocket"){
  preopened(newfd,true);
//save other stuff for debug:
  connectArgs.connected = sadr;
  portnumber = sadr.getPort();
  connected = isOpen();
}

///////////////////////////////

bool HostInfo::lookup(const char *name, unsigned port, const char *service){
  getError = getaddrinfo(name, service,&hints,&res);
  gotten = res!=nullptr;
  lastErrno = getError ? errno : 0;
  if(getError) {
    return false;
  } else {
    if(service==nullptr) {
      if(port) {
        unsigned curious(0);
        //inject port number into addr structs in the list.
        //that involves knowing what kind of address each is.
        unsigned hostport = htons(port);
        for(addrinfo*ai = res; ai; ai = ai->ai_next) {
          if(ai->ai_family==AF_INET) {
            lastipv4 = curious;
            sockaddr_in &host( *reinterpret_cast<sockaddr_in *>( ai->ai_addr));
            host.sin_port = hostport;
          } else if(ai->ai_family==AF_INET6) {
            sockaddr_in6 &host( *reinterpret_cast<sockaddr_in6 *>( ai->ai_addr));
            host.sin6_port = hostport;
          }
          ++curious;
        }
        dbg("There were %d choices of addrinfo",curious);
      }
    }
    return true;
  }
} // HostInfo::lookup

addrinfo *HostInfo::bestAddress(){
  for(addrinfo*ai = res; ai; ai = ai->ai_next) {
    if(ai->ai_family==AF_INET) {
      return ai;
    }
  }
  return res;
}

void HostInfo::keep(addrinfo *one){
  if(one==nullptr) {
    one = res;
  }
  if(one==nullptr) {
    return;
  }
  memmove(&connected.address,one->ai_addr,connected.length = one->ai_addrlen);
}

const char *HostInfo::errorText(const char *ifOk){
  if(getError) {
    if(getError==EAI_SYSTEM) {
      return strerror(lastErrno);
    }
    return gai_strerror(getError);
  } else {
    return ifOk;
  }
}

void HostInfo::free(){
  if(flagged(gotten)) {
    freeaddrinfo(res);
  } else {
    delete res;//pathological
  }
  res = nullptr;//improve detection of use after free
}

void HostInfo::clip(){
//todo: detect which member we use, free the others and set res to it. Until then we use the first and ditch the rest.
  if(gotten && res && res->ai_next) {
    freeaddrinfo(res->ai_next);
  }
}

HostInfo::HostInfo(){
  hint();
}

HostInfo::~HostInfo(){
  free();
}

void HostInfo::hint(bool tcp){
  hints.ai_family = AF_UNSPEC;//   AF_INET and AF_INET6.
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;// SOCK_STREAM , someday: SOCK_DGRAM, 0 for any
  hints.ai_protocol = tcp ? 6 : 17;  //6 to limit to TCP, 17 for UDP else see https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
  hints.ai_flags = 0; //todo:1 systematize for numeric name

  //the rest are zeroed for safety.
  hints.ai_addrlen = 0;
  hints.ai_addr = nullptr;
  hints.ai_canonname = nullptr;
  hints.ai_next = nullptr;
}

addrinfo *HostInfo::anIpv4() {
  return this->bestAddress();//todo: what was intended here?
}
// HostInfo::hint

unsigned SockAddress::getPort(){
  if(address.sa_family==AF_INET) {
    sockaddr_in &sin(*reinterpret_cast<sockaddr_in*>(&address));
    return ntohs(sin.sin_port);
  } else {
    return BadIndex;
  }
}

unsigned SockAddress::getIpv4(){
  if(address.sa_family==AF_INET) {
    sockaddr_in &sin(*reinterpret_cast<sockaddr_in*>(&address));
    return ntohl(sin.sin_addr.s_addr);
  } else {
    return BadIndex;//todo:1 this is NOT a bad ip address (it is a broadcast), need to get that from some RFC.
  }
}
