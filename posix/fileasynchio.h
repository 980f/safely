#ifndef FileAsyncAccess_H
#define FileAsyncAccess_H

#include "incrementalfiletransfer.h"
#include "aio.h"
#include "signal.h"
#include "fildes.h" //our file <-> safe buffer routines
#include "hook.h"  //used for callbacks to process file.
/** mate aio calls to FilDes class
 * Note that the file is left open at the end of transfer, you can chain more data on writes.
 * The file will be closed when the FileAsyncAccess is deleted. */
class FileAsyncAccess: public IncrementalFileTransfer {

  /** the thing we are wrapping use of: */
  aiocb cb;

public:
  /* connect to user allocated file and buffer objects. This makes it easier to change synch code to async. */
  FileAsyncAccess(bool reader,Fildes &fd,ByteScanner &buf);
  /** start read process */
  bool go();//reserve place for a null

  /** For polling @returns whether background operation is not still in progress */
  bool isDone() const {
    int ercode=aio_error (&cb); //bypassing PosixWrapper for no documented reason :(
    return ercode!=EINPROGRESS;
  }
  /** @returns not reported as done and more bytes expected */
  bool notDone()const;

  /** block, used only for diagnostics on this class. @returns whether aio_suspend 'failed'.
   * I quote 'failed' since it includes things like whether it quit because a signal occured.
   * One of the failures is (on read) whether there was not enough source data to completely fill the buffer.
   * This could also be called 'poll with timeout'
   * @returns whether block() executed ok, which is ambiguous when the last transfer is a partial buffer */
  bool block(double seconds);

  void loiter();

private:
  /** this will call back to our member fn */
  static void sighandler(int signo, siginfo_t *info, void *);
  /* @param code is siginfo_t.si_code */
  void notified(int code, int ernumber);
  /* rerunnable part of operation */
  bool launch(bool more);

  // IncrementalFileTransfer interface
public:
  virtual bool onEachBlock(__ssize_t amount) override;
  virtual void onDone() override;
};

#endif // FILEREADER_H
