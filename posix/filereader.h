#ifndef FILEREADER_H
#define FILEREADER_H

#include "aio.h"
#include "signal.h"

//our file <-> safe buffer routines
#include "fildes.h"
#include "hook.h"
/** mate aio calls to FilDes class, initially for reader, will likely rename and refactor etc. */
class FileReader: public PosixWrapper {
  /** the file */
  Fildes &fd;
  /** the data source */
  ByteScanner &buf;//todo: replace with Indexer<u8> throughout hierarchy
  unsigned guard;
  typedef Hooker<bool /*go again*/,__ssize_t /*exitcode or number of bytes read */> OnCompletion;
  /** what to call when operation is completed.
   * If the operation succeeded and this function returns true then a new read is launched.
   * When the operation fails this is called with an error code (value <0) and the return is ignored. */
  OnCompletion continuation;

  /** the thing we are wrapping use of: */
  aiocb cb;
  /** this will call back to our member fn */
  static void sighandler(int signo, siginfo_t *info, void *);
  /* @param code is siginfo_t.si_code */
  void notified(int code, int ernumber);
public:
  FileReader(Fildes &f);
public:
  FileReader(Fildes &fd,ByteScanner &buf,OnCompletion::Pointer onDone);
  /** start read process */
  bool go(unsigned guard=1);//reserve place for a null

  /** having trouble sometimes putting this in constructor arg list: */
  void setHandler(OnCompletion::Pointer onDone);

  /** pollable bit*/
  bool isDone() const {
    int ercode=aio_error (&cb);
    return ercode!=EINPROGRESS;
  }

  /** block, used only for diagnostics on this class. @returns whether aio_suspend 'failed'.
   * I quote 'failed' since it includes things like whether it quit because a signal occured. */
  bool block(double seconds);

private:
  /* rerunnable part of operator () */
  bool launch();
};

#endif // FILEREADER_H
