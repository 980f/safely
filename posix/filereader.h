#ifndef FILEREADER_H
#define FILEREADER_H

#include "aio.h"
#include "signal.h"

//our file <-> safe buffer routines
#include "fildes.h"
#include "hook.h"
/** mate aio calls to FilDes class, initially for reader, will likely rename and refactor etc. */
class FileReader {
  /** the file */
  Fildes &fd;
  /** the data source */
  ByteScanner &buf;//todo: replace with Indexer<u8> throughout hierarchy
  typedef Hooker<bool /*go again*/,size_t /*exitcode or number of bytes read */> OnCompletion;
  /** what to call when operation is completed.
   * If the operation succeeded and this function returns true then a new read is launched.
   * When the operation fails this is called with an error code (value <0) and the return is ignored. */
  OnCompletion continuation;

  /** the thing we are wrapping use of: */
  aiocb cb;
  /** this will call back to our member fn */
  static void sighandler(int signo, siginfo_t *info, void *);
  /* @param code is siginfo_t.si_code */
  void notified(int code, int errno);
public:
  FileReader(Fildes &fd,ByteScanner &buf,OnCompletion::Pointer onDone);
  /** start read process */
  bool operator()();
private:
  void launch();
};

#endif // FILEREADER_H