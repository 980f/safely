#  pragma once
//  "(C) Andrew L. Heilveil, 2017"

#include "incrementalfiletransfer.h"
#include "aio.h"
#include <csignal>
#include "fildes.h" //our file <-> safe buffer routines
#include "hook.h"  //used for callbacks to process file.
/** mate aio calls to FilDes class
 * Note that the file is left open at the end of transfer, you can chain more data on writes.
 * The file will be closed when the FileAsyncAccess is deleted. */
class FileAsyncAccess : public IncrementalFileTransfer {
  /** the thing we are wrapping use of: */

  struct Aiocb: aiocb {
    Aiocb() {
      EraseThing(*this);
    }
  } cb;

public:
  /* connect to user allocated file and buffer objects. This makes it easier to change synch code to async. */
  FileAsyncAccess(bool reader); // NOLINT(*-explicit-constructor)
  /** start read process */
  bool go();

  /** For polling @returns whether background operation is not still in progress */
  bool isDone() const {
    int ercode = aio_error(&cb); //bypassing PosixWrapper for no documented reason :(
    return ercode != EINPROGRESS;
  }

  /** @returns not reported as done and more bytes expected */
  bool notDone() const;

  /** block, used only for diagnostics on this class. @returns whether aio_suspend 'failed'.
   * I quote 'failed' since it includes things like whether it quit because a signal occured.
   * One of the failures is (on read) whether there was not enough source data to completely fill the buffer.
   * This could also be called 'poll with timeout'
   * @returns whether block() executed ok, which is ambiguous when the last transfer is a partial buffer */
  bool block(double seconds);

  /** similar to block this polls for completion, but doesn't suspend the thread. This is for testing. */
  void loiter();

  /** issue a cancel request, man aio_cancel for full behavior */
  void cancel();

private:
  /** this will call back to our member fn */
  static void sighandler(int signo, siginfo_t *info, void *);

  /* @param code is siginfo_t.si_code */
  void notified(int code, int ernumber);

  /* re-runnable part of operation */
  bool launch(bool more);

  // IncrementalFileTransfer interface
public:
  bool onEachBlock(__ssize_t amount) override;

  void onDone() override;
};
