#include "fildes.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "fdset.h"

// just for name research
#include <linux/limits.h>

#include "charformatter.h"

Fildes::Fildes(const char *whatfor) : PosixWrapper(whatfor) {
  errornumber = 0;
  lastRead = lastWrote = 0;
  fd = BADFD;
  amOwner = false;
}

Fildes::Fildes(const Fildes &other) : Fildes(other.dbg.prefix) {
  this->fd = other.fd;
  this->name.copy(other.name); // need independent copy for safety.
}

bool Fildes::assignFd(int anFD) {
  if (fd != anFD) {
    beforeChange(anFD);
    fd = anFD;
    lastRead = lastWrote = 0;//don't trust overloads to get back to us for this.
    afterChange();
  }

  // no, might call another member which generates an error while generating the fd: errornumber=0;
  return isOpen();
}

/** since we close on going out of scope if you share an fd you must take care to use pointer or reference*/
Fildes::~Fildes() {
  if (amOwner) {
    close();
  }
}

bool Fildes::open(const char *devname, int O_stuff) { // todo:3 expose 3rd argument
  close(); // is a smart close, ignore any errors
  amOwner = true;
  int maybefd;
  if (okValue(maybefd, ::open(devname, O_stuff, 0777))) { // 3rd arg is only relevant if O_stuff includes O_Creat. The (3) 7's lets umask provide the argument.)
    assignFd(maybefd);
    name = devname; // there's a strdup in here.
    return true;
  } else {
    dbg("Failed to open %s", devname);
    return false;
  }
} // Fildes::open

FILE *Fildes::getfp(const char *fargs) {
  return ::fdopen(fd, fargs ? fargs : "r"); // todo:2 make string to match present state of fd's flags
}

unsigned Fildes::available() { // was buggy prior to 25apr2018
  unsigned bytesAvailable = 0;
  if (failure(ioctl(FIONREAD, &bytesAvailable))) { // missing third arg did amazing damage to caller
    return 0; // a place to breakpoint
  } else {
    return bytesAvailable;
  }
}

bool Fildes::preopened(int fd, bool urit) {
  close();
  amOwner = urit;
  return assignFd(fd);
}

bool Fildes::setBlocking(bool block) {
  return setSingleFlag(O_NONBLOCK, !block);
}

void Fildes::Close(int &somefd) {
  ::close(somefd);
  somefd = BADFD;
}

Text &Fildes::getName() {
  // proc/self/fdastext resolve link
  char seeker[100];
  CharFormatter procself(seeker, sizeof seeker);
  procself.cat("/proc/self/");
  procself.printNumber(fd);
  procself.next() = 0;

  thread_local char temp[PATH_MAX + 1]; //+1 COA.
  CharScanner response(temp, sizeof(temp));
  response.zguard();
  auto target = procself.internalBuffer(); // 4 debug
  auto actualsize = readlink(target, response.internalBuffer(), response.freespace());
  if (actualsize > 0) { // always get -1 here, errno:0
    response.skip(unsigned(actualsize));
    response.next() = 0;
    name = response.internalBuffer();
  } else {
    // name is still what 'open' was given.
  }
  return name;
} // Fildes::getName

bool Fildes::setSingleFlag(int bitfield, bool one) {
  if (!isOpen()) {
    return false;
  }
  int flags;
  if (okValue(flags, fcntl(fd, F_GETFL, 0))) {
    int existing = flags;
    if (one) {
      flags |= bitfield;
    } else {
      flags &= ~bitfield;
    }
    if (changed(existing, flags)) {
      return ok(fcntl(fd, F_SETFL, flags));
    } else {
      return true; // didn't need to change.
    }
  } else {
    return false;
  }
} // Fildes::setSingleFlag

bool Fildes::getSingleFlag(int bitfield, bool &bit) {
  if (!isOpen()) {
    return false;
  }
  int flags;
  if (okValue(flags, fcntl(fd, F_GETFL, 0))) {
    bit = (flags & bitfield) != 0;
    return true;
  } else {
    return false;
  }
} // Fildes::getSingleFlag

int Fildes::close() {
  if (amOwner && isOpen()) {
    amOwner = false;
    beforeChange(BADFD);
    return ::close(postAssign(fd, BADFD)); // when we close an fd someone else can claim it.
  } else {
    return 0; // not an error to close something that isn't open
  }
}

bool Fildes::mark(FDset &bitset) const {
  if (isOpen()) {
    return bitset.include(fd);
  } else {
    return bitset.exclude(fd);
  }
}

bool Fildes::isMarked(const FDset &fdset) const {
  return isOpen() && fdset.includes(fd);
}

bool Fildes::read(uint8_t *buf, unsigned len) {
  if (isOpen()) {
    if (okValue(lastRead, ::read(fd, buf, len))) {
      if (traceRead) {
        return true;
      }
      return true;
    } else {
      if (isWaiting()) {
        lastRead = 0;
        return true;
      } else {
        return false;
      }
    }
  } else {
    lastRead = BadSize; // todo:2 ensure errno is 'file not open'
    return false;
  }
} // Fildes::read

bool Fildes::read(Indexer<uint8_t> &p) {
  if (read(&p.peek(), p.freespace())) {
    if (lastRead >= 0) {
      p.skip(lastRead);
    }
    return true;
  } else {
    return false;
  }
} // Fildes::read

bool Fildes::read(Indexer<char> &p) {
  if (read(reinterpret_cast<uint8_t *>(&p.peek()), p.freespace())) {
    if (lastRead >= 0) {
      p.skip(lastRead);
    }
    return true;
  } else {
    return false;
  }
} // Fildes::read

bool Fildes::write(Indexer<uint8_t> &p) {
  if (write(&p.peek(), p.freespace())) {
    if (lastWrote >= 0) {
      p.skip(lastWrote);
    }
    return true;
  }
  return false;
}

bool Fildes::write(Indexer<uint8_t> &&p) {
  if (write(&p.peek(), p.freespace())) {
    if (lastWrote >= 0) {
      p.skip(lastWrote);
    }
    return true;
  }
  return false;
}

bool Fildes::write(Indexer<char> &p) {
  if (write(reinterpret_cast<const uint8_t *>(&p.peek()), p.freespace())) {
    if (lastWrote >= 0) {
      p.skip(lastWrote);
    }
    return true;
  }
  return false;
}

bool Fildes::write(Indexer<char> &&p) {
  if (write(reinterpret_cast<const uint8_t *>(&p.peek()), p.freespace())) {
    if (lastWrote >= 0) {
      p.skip(lastWrote);
    }
    return true;
  }
  return false;
}

bool Fildes::write(const uint8_t *buf, unsigned len) {
  if (isOpen()) {
    if (okValue(lastWrote, ::write(fd, buf, len))) {
      if (traceWrite) {
        return true; // this is a place to breakpoint on when you want to see every write that works.
      }
      return true;
    }
    return false;
  } else {
    lastWrote = BadSize;
    return false;
  }
} // Fildes::write

bool Fildes::writeChars(char c, unsigned repeats) {
  if (repeats <= 4096) {
    uint8_t reps[repeats];
    fillObject(reps, repeats, uint8_t(c)); // sizeof(reps) always gave 1
    return write(reps, repeats);
  } else {
    return false;
  }
}

// todo:3 configuration parameter for how much we are willing to transfer before checking other channels
#define CHUNK 3000

int Fildes::moveto(Fildes &other) {
  // This presumes that write copies to an internal buffer before returning.
  uint8_t localbuffer[CHUNK];

  ByteScanner wrapper(localbuffer, sizeof(localbuffer));
  //todo:00 this is very broken! It has been hacked to compile as we are going to use asynchio instead of polling.
  if (isOpen()) {
    int got = read(wrapper) ? wrapper.used() : 0;
    if (got > 0) { // write to otherfd, nonblocking!
      int put = other.isOpen() && other.write(wrapper) ? wrapper.used() : false;
      if (put < 0) { // device has a problem.
        return -2;
      }
      if (got > wrapper.used()) { // on incomplete write
        return got - wrapper.used(); // note it and proceed, else we would have to add extra buffering herein.
      }
    }
    if (got < 0) { // then somehow the device got closed.
      setFailed(true);
      return -1;
    }
  }
  return 0;
} // Fildes::moveto

// end of file
