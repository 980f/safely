#ifndef POSIXWRAPPER_H
#define POSIXWRAPPER_H

/**
 * handy things for wrapping a POSIX C API with a C++ class, if only for errno management.
 */
class PosixWrapper {
protected:
  bool failure(bool passthrough = false); //#legacy default arg
  /** test return value for "ok", log error code*/
  bool failed(int zeroorminus1);
public:
  /** ERRNO for last operation done using the extend object */
  int errornumber;
  /** logging noisiness for the (extended) object */
  int debug;
public:
  PosixWrapper();
  /** printf like logging, with all the faults therein.*/
  void logmsg(const char *fmt, ...);
}; // class PosixWrapper

#endif // POSIXWRAPPER_H
