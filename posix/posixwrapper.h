#ifndef POSIXWRAPPER_H
#define POSIXWRAPPER_H

/**
  * handy things for wrapping a POSIX C API with a C++ class,
  * especially fine grained printf based logging.
  */

class PosixWrapper {
protected:
  bool failure(bool passthrough = false); //#legacy default arg
  /** test return value for "ok", log error code*/
  bool failed(int zeroorminus1);
public:
  int errornumber;  //attempt at thread-safe errno tracking.
  int debug;        //logging noisiness for the (extended) class
public:
  PosixWrapper();

  void logmsg(const char *fmt, ...);
};


#endif // POSIXWRAPPER_H
