#ifndef FILDES_H
#define FILDES_H

#include "charscanner.h"   //safe buffer
#include "posixwrapper.h"  //manage errno
struct FDset;//forward reference, we may be relocating this logic elsewhere as we purge select() in favor of poll() #include "fdset.h"
#include "stdio.h"  //FILE

/** wrapper around file descriptors, especially noteworthy is that it closes the file on destruction, so best use is to create and use locally.*/
class Fildes : public PosixWrapper {
public:
  //make a true variable for something that is usuall #defined.
  static const int BADFD= -1;
  //retain for post-mortem debug.
  int lastRead;
  int lastWrote;
protected:
/** whether this object opened the fd it wraps. That is the normal case but if you want to do multiple operations and retain error info on each step then you might use multiple Fildes objects around the same fd. */
  bool amOwner;
  int fd;
  bool assignFd(int anFD);
public:
  Fildes();
  /** since we close on going out of scope if you share an fd you must take care to use pointer or reference*/
  ~Fildes();
  bool open(const char *devname, int O_stuff); //open a named file
  /** takes ownership of an FD, if @param urit is true ("You are it")*/
  bool preopened(int fd,bool urit=true);
  /** set/clear a fcntl accessible flag, @returns success of operation */
  bool setSingleFlag(int bitfield, bool one) const;
  int close(void);
  /**make this transparently usable as an fd number*/
  operator int() const {
    return fd;
  }
  /** to get rid of warnings when compiler can't figure out an automatic cast to int.*/
  int asInt() const {
    return fd;
  }

  bool isOpen() const {
    return fd != BADFD;
  }

  /** get alternative view of the file.
@returns a FILE pointer for the same file */
  FILE * getfp(const char *fargs=nullptr);

  /** read from used bytes of buffer */
  int read(ByteScanner&p);
  /** write to remaining free bytes of buffer */
  int write(ByteScanner&p);

  /** @returns isOpen()*/
  bool mark(FDset&fdset) const;
  /** @returns whether bit associated with this is a one in the fdset.*/
  bool isMarked(FDset&fdset) const;
  /** moves all bytes pending on this' OS read buffer to the other file.
    *
    * @returns 0 on full success, a positive number if some bytes are dropped, -1 for read error (see the fd's lastRead for details) -2 for write error (see that fd's lastWrote for detail. */
  int moveto(Fildes&other);
  /**set file to either blocking or not blocking */
  bool setBlocking(bool block) const;
};

#endif // FILDES_H
