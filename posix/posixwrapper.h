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
  /** ERRNO for last operation done using the extend object */
  int errornumber;  //attempt at thread-safe errno tracking.
  /** //logging noisiness for the (extended) object */
  int debug;
public:
  PosixWrapper();
  /** printf like logging, with all the faults therein.*/
  void logmsg(const char *fmt, ...);
};

#endif // POSIXWRAPPER_H
