#ifndef FileAsyncAccess_H
#define FileAsyncAccess_H

#include "aio.h"
#include "signal.h"


#include "fildes.h" //our file <-> safe buffer routines
#include "hook.h"  //used for callbacks to process file.
/** mate aio calls to FilDes class, initially for reader, will likely rename and refactor etc.
 * This base is geared towards whole file read and writes, I will add classes for 'big file' reading/writing.
*/
class FileAsyncAccess: public PosixWrapper {
  /** transfer direction. */
  const bool amReader;//const until we can make sense of changing direction while open.
  /** the file */
  Fildes &fd;
  /** a view into the data source or sink */
  ByteScanner &buf;

  typedef Hooker<bool /*go again*/,__ssize_t /*exitcode or number of bytes read */> OnCompletion;
  /** what to call when operation is completed.
   * If the operation succeeded and this function returns true then a new read is launched.
   * When the operation fails this is called with an error code (value <0) and the return is ignored. */
  OnCompletion continuation;

  /** the thing we are wrapping use of: */
  aiocb cb;

public:
  /* connect to user allocated file and buffer objects. This makes it easier to change synch code to async. */
  FileAsyncAccess(bool reader,Fildes &fd,ByteScanner &buf,OnCompletion::Pointer onDone);
  /** start read process */
  bool go();//reserve place for a null

  /** having trouble sometimes putting this in constructor arg list: */
  void setHandler(OnCompletion::Pointer onDone);

  /** For polling @returns whether background operation is not still in progress */
  bool isDone() const {
    int ercode=aio_error (&cb);
    return ercode!=EINPROGRESS;
  }

  /** block, used only for diagnostics on this class. @returns whether aio_suspend 'failed'.
   * I quote 'failed' since it includes things like whether it quit because a signal occured.
   * One of the failures is (on read) whether there was not enough source data to completely fill the buffer.
   * This could also be called 'poll with timeout' */
  bool block(double seconds);

private:
  /** this will call back to our member fn */
  static void sighandler(int signo, siginfo_t *info, void *);
  /* @param code is siginfo_t.si_code */
  void notified(int code, int ernumber);
  /* rerunnable part of operation */
  bool launch(bool more);
};

#endif // FILEREADER_H
