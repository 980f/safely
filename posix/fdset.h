#ifndef FDSET_H
#define FDSET_H

#include "posixwrapper.h"
#include <sys/select.h>
/** wrapper around use of select() to wait for an event.
 *  todo: the return vlaue for include and exclude mixes 'bad fd' and 'none set' into one boolean, that might be a bad design choice since nonTrivial() can be called independently.
*/
class FDset {
protected:
  int maxfd;
  fd_set group;
  /** not public as it only tests validity for setting in a mask, not any other sense of 'valid'*/
  static bool validFd(int fd);
public:
  FDset();
  void clear();
  bool notTrivial() const;
  /** @returns whether the given fd is a valid value, in which case it has been recorded in this FDset */
  bool include(int fd);
  /** @returns false when the given fd is invalid or the resulting mask has no bits set after a valid fd has been removed */
  bool exclude(int fd);
  /** @returns whether this set includes the mentiond fd*/
  bool includes(int fd) const;
};

/** slightly restricted access to 'select', the same fds are used for read,write,and error, you can't listen to read on one fd and write on another.
 *  This limitation is fine for the very common use cases of only one file being of interest and a pair of which only one will be reading and the other writing, while errors on either is of equal interest.
 *  todo:2 constructor which selects() on construction using an fd and a timeout.
 */
class SelectorSet : public FDset,PosixWrapper {
private:
  /** if check then copy group to given worker, else return a null pointer*/
  fd_set *prepared(fd_set&fds, bool check);
  fd_set readers;
  fd_set writers;
  fd_set troublers;  
  struct timeval timeout;
public:
  SelectorSet();

  /** @param rwe: any combo of r for reads w for writes e for errors.
   *  @returns whatever select() returns when that is without error, else records error and returns 0
   */
  int select(const char *rwe = "rwe");
  /** The isxxx methods only make sense after returning from select(), prior to that they are the same as the base class group's nonTrivial(). */
  bool isReadable(int fd) const;
  bool isWritable(int fd) const;
  bool isTroubled(int fd) const;
  /** set timeout in <b>seconds</b>, not ms. */
  void setTimeout(double seconds);
};

#endif // FDSET_H
