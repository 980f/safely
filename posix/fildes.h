#pragma once
// "(C) 2024(and bits earlier) Andrew L.Heilveil, github / 980f"

#include "buffer.h" //safe buffer
#include "posixwrapper.h" //manages errno and logging
struct FDset; // forward reference, we may be relocating this logic elsewhere as we purge select() in favor of poll() #include "fdset.h"
#include <cstdio> //FILE
#include <sys/ioctl.h> //we wrap ioctl with a template, would have to gyrate quite a bit to isolate including this header to the cpp, could be done with some nasty typecasting.
#include <textpointer.h>

/** wrapper around OS file descriptors.
 * Especially noteworthy is that it closes the file on destruction, so best use is to create and use locally, as best use is to open a file and consume it all right away, else use asynch io.
 *
 * It includes the concept of 'owning' the fd, so that only the owning class will do things like close the file.
 * It retains a lot of debug info, such as independent record of last read and write results.
 */
class Fildes : public PosixWrapper {
public:
  // make a true variable for something that is usually #defined:
  static const int BADFD = ~0;
  static const ssize_t BadSize = ~0;
  // retain for post-mortem debug. using practical type vs posix type to minimize compiler warnings
  ssize_t lastRead;
  ssize_t lastWrote;
  // debug aid
  bool traceRead = false;
  bool traceWrite = false;

protected:
  /** whether this object opened the fd it wraps. That is the normal case but if you want to do multiple operations and retain error info on each step then you might use multiple Fildes objects around the same fd. */
  bool amOwner; // determines whether we auto close on destruction.
  int fd;

  bool assignFd(int anFD);

public:
  explicit Fildes(const char *whatfor);

  /** copies ONLY the fd, none of the other state, and most especially is NOT the owner of the underlying file descriptor */
  Fildes(const Fildes &other);

  /** since we close on going out of scope if you share an fd you must take care to use pointer or reference*/
  virtual ~Fildes();

  bool open(const char *devname, int O_stuff); // open a named file
  /** takes ownership of  @param fd  if @param urit is true ("You are it"), else just copy it and probably should not call close() as that will piss off the actual owner, making it lose track of the fd's state. */
  bool preopened(int fd, bool urit = true);

  /** set/clear a fcntl accessible flag, @returns success of operation */
  bool setSingleFlag(int bitfield, bool one);

  /** @param bit is true if the @param bitfield flag is set, @returns whether bit was updated.
   * If you cheat and send a multi-bit bitfield you'll have to read the code to see what happens. */
  bool getSingleFlag(int bitfield, bool &bit);

  /** close the fd IFFI we own it and we think that it is open.
   * @returns what system's close returns.
   * @note the auto close on destruction does not call overloads, only this exact function.
   */
  virtual int close();

  /**make this transparently usable as an fd number*/
  operator int() const { // #~intentionally implicit NOLINT(*-explicit-constructor)
    return fd;
  }

  /** to get rid of warnings when compiler can't figure out an automatic cast to int, such as var-arg list.*/
  int asInt() const {
    return fd;
  }


  /** @returns a very weak sense of whether the fd is open, more like "we asked for it to be open and haven't noticed it closing"   */
  bool isOpen() const {
    return fd != BADFD;
  }

  /** get alternative view of the file.
   *  @returns a FILE pointer for the same file */
  FILE *getfp(const char *fargs = nullptr);

  /** @returns the number of bytes available for reading. Not all types of fd will return something useful here. */
  unsigned available(); // const was removed so that errors can be recorded.

  /** read into freespace/tail of buffer */
  bool read(Indexer<uint8_t> &p);

  bool read(Indexer<char> &p);

  bool read(uint8_t *buf, unsigned len);

  /** write from freespace/tail of buffer.
   * This choice is due to supporting partial sends, updating the referenced pointer as we progress.
   * When you are finished building something to send you create a new Indexer object from the head of that one.
   */
  bool write(Indexer<uint8_t> &p);

  bool write(Indexer<uint8_t> &&p);

  bool write(Indexer<char> &p);

  bool write(Indexer<char> &&p);

  bool write(const uint8_t *buf, unsigned len); // placeholder
  /** write a character a bunch of times. Handy for things like indenting a nested text printout. */
  bool writeChars(char c, unsigned repeats); // adding a default arg makes this and write(Indexer<uint8_t>) ambiguous

  /** set the associated flag given this guy's fd.
   * @returns @see isOpen() */
  bool mark(FDset &fdset) const;

  /** @returns whether bit associated with this guy is a one in the fdset.*/
  bool isMarked(const FDset &fdset) const;

  /** moves all bytes pending on this' OS read buffer to the other file.
   *
   * @returns 0 on full success, a positive number if some bytes are dropped, -1 for read error (see the fd's lastRead for details) -2 for write error (see that fd's lastWrote for detail. */
  int moveto(Fildes &other);

  /**set file to either blocking or not blocking */
  bool setBlocking(bool block);

  /** close it then forget which it was */
  static void Close(int &somefd);

  /** thin wrapper around ioctl calls */
  template<typename Scalar> bool ioctl(unsigned code, Scalar datum) {
    // pre-test isopen so as to not overwrite the error code from that.
    return isOpen() && ok(::ioctl(asInt(), code, datum));
  }

  template<typename Pod> Pod read(const Pod marker) {
    Pod result; //don't try to make 'marker' a Pod, we might get a partial read and then we would have corruption.
    if (read(reinterpret_cast<uint8_t *>(&result), sizeof(Pod))) {
      if (sizeof(Pod) == lastRead) {
        return result;
      }
    }
    return marker;
  }

  template<typename Pod> bool write(Pod &&datum) {
    if (write(reinterpret_cast<uint8_t *>(&datum), sizeof(Pod))) {
      if (sizeof(Pod) == lastWrote) {
        return true;;
      }
    }
    return false;
  }

protected:
  Text name; // not always valid, call getName to ensure.
public:
  /** recover name from OS. Uses linux PROC file system specific routine, and is not cheap. */
  Text &getName();
}; // class Fildes
