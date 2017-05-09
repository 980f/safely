#include "fildes.h"

#include "cheaptricks.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "fdset.h"
#include "errno.h"

Fildes::Fildes(const char *whatfor):PosixWrapper (whatfor){
  errornumber = 0;
  lastRead = lastWrote = 0;
  fd = BADFD;
  amOwner = false;
}

bool Fildes::assignFd(int anFD){
  this->fd = anFD;
  lastRead = lastWrote = 0;
  //no, might call another member which generates an error while generating the fd: errornumber=0;
  return isOpen();
}

/** since we close on going out of scope if you share an fd you must take care to use pointer or reference*/
Fildes::~Fildes(){
  if(amOwner) {
    close();
  }
}

bool Fildes::open(const char *devname, int O_stuff){//todo:3 expose 3rd argument
  close();//is a smart close, ignore any errors

  amOwner = true;
  int maybefd;
  if(okValue(maybefd,::open(devname, O_stuff,0777))){//3rd arg is only relevant if O_stuff includes O_Creat. The (3) 7's lets umask provide the argument.)
    assignFd(maybefd);
    return true;
  } else {
    dbg("Failed to open %s",devname);
    return false;
  }
}

FILE *Fildes::getfp(const char *fargs){
  return ::fdopen(fd,fargs ?: "r");//todo:2 make string to match present state of fd's flags
}

bool Fildes::preopened(int fd,bool urit){
  close();
  amOwner = urit;
  return assignFd(fd);
}

bool Fildes::setBlocking(bool block) const {
  return setSingleFlag(O_NONBLOCK, !block);
}

void Fildes::Close(int &somefd){
  ::close(somefd);
  somefd=BADFD;
}

bool Fildes::setSingleFlag(int bitfield, bool one) const {
  if(!isOpen()) {
    return false;
  }
  int flags;
  flags = fcntl(fd, F_GETFL, 0);
  if(flags != -1) {
    if(one) {
      flags |= bitfield;
    } else {
      flags &= ~bitfield;
    }
    fcntl(fd, F_SETFL, flags);
    return true;
  } else {
    return false;
  }
} /* setSingleFlag */

int Fildes::close(void){
  if(amOwner&&isOpen()) {
    amOwner = false;
    return ::close(fd);
  } else {
    return 0; //not an error to close something that isn't open
  }
}

bool Fildes::mark(FDset&bitset) const {
  if(isOpen()) {
    return bitset.include(fd);
  } else {
    return bitset.exclude(fd);
  }
}

bool Fildes::isMarked(const FDset&fdset) const {
  return isOpen() && fdset.includes(fd);
}

int Fildes::read(Indexer<u8> &p){
  if(isOpen()) {
    if(okValue(lastRead ,::read(fd, &p.peek(), p.freespace()))) {
      p.skip(lastRead);
    }
    return lastRead;
  } else {
    return lastRead = -1;//todo:2 ensure errno is 'file not open'
  }
} // Fildes::read

int Fildes::write(Indexer<u8>&p){
  if(isOpen()) {
    if(okValue(lastWrote, ::write(fd, &p.peek(), p.freespace()))) {
      p.skip(lastWrote);
    }
    return lastWrote;
  } else {
    return lastWrote = -1; //todo:2 error code
  }
}

int Fildes::write(const u8 *buf, unsigned len){
  if(isOpen()) {
    if(okValue(lastWrote, ::write(fd,buf, len))) {
//      p.skip(lastWrote);
    }
    return lastWrote;
  } else {
    return lastWrote = -1; //todo:2 error code
  }
} // Fildes::write

//todo:3 configuration parameter for how much we are willing to transfer before checking other channels
#define CHUNK 3000

int Fildes::moveto(Fildes&other){
  // This presumes that write copies to an internal buffer before returning.
  u8 localbuffer[CHUNK];

  ByteScanner wrapper(localbuffer, sizeof(localbuffer));

  if(isOpen()) {
    int got = read(wrapper);
    if(got > 0) { //write to otherfd, nonblocking!
      int put = other.isOpen() ? other.write(wrapper) : 0;
      if(put < 0) {
        //device has a problem.
        return -2;
      }
      if(got > put) { //on incomplete write
        return got - put; //note it and proceed, else we would have to add extra buffering herein.
      }
    }
    if(got < 0) { //then somehow the device got closed.
      setFailed(true);
      return -1;
    }
  }
  return 0;
} /* moveto */

//end of file
