#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "fileasynchio.h"

class FileWriter{
  Fildes fd;
protected:
  ByteScanner buf;

  FileAsyncAccess fmover;

//protected://starting with overloads, will replace with delegates once it is tested.
//  virtual bool action();
//  virtual void onCompletion();
public:
  FileWriter();
  virtual ~FileWriter();//ensure fildes is released.
  bool process(TextKey fname, const ByteScanner &source);
  /** for polling, indicates bytes not yet definitely sent, they may have been passed to the OS but the OS hasn't told us that it has dealt with them.*/
  unsigned remaining()const;
private:
  /* on incremental write complete */
  bool onWrite(__ssize_t ret);
};

#endif // FILEREADERTESTER_H
