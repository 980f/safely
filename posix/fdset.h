#ifndef FDSET_H
#define FDSET_H

#include <sys/select.h>
/** wrapper around use of select() to wait for an event. */
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
  bool include(int fd);
  bool exclude(int fd);
  bool includes(int fd) const;
};

class SelectorSet : public FDset {
private:
  /** if check then copy group to given worker, else return a null pointer*/
  fd_set *prepared(fd_set&fds, bool check);
  fd_set readers;
  fd_set writers;
  fd_set troublers;
public:
  struct timeval timeout;
  SelectorSet(); //todo: encapsulate setting poll time
  /** @param rwe: any combo of r for reads w for writes e for errors*/
  int select(const char *rwe = "re");
  bool isReadable(int fd) const;
  bool isWritable(int fd) const;
  bool isTroubled(int fd) const;
};

#endif // FDSET_H
