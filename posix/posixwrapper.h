#ifndef POSIXWRAPPER_H
#define POSIXWRAPPER_H

/**
 * handy things for wrapping a POSIX C API with a C++ class, if only for errno management.
 * Fairly recent errno implementations are per-thread, but even so one has to store away the errno
 * after every operation that affects it. This class is handy for the latter.
 *
 * This also wraps use of the syslog facility.
 */
class PosixWrapper {
protected:
  bool failure(bool passthrough = false); //#legacy default arg
  /** test posix function return value for "ok". log error code if not.
    this updates errornumber member on each call, but only emits a system log message if the errno has changed since the last time a message was sent.*/
public: //so that we can merge
  bool failed(int zeroorminus1);
  /** syntactic sugar for !@see failed() */
  bool ok(int zeroorminus1){
    return !failed(zeroorminus1);
  }

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
