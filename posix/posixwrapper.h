#ifndef POSIXWRAPPER_H
#define POSIXWRAPPER_H

#include "errno.h" //because we templated a function that needs it
#include "logger.h"
/**
 * handy things for wrapping a POSIX C API with a C++ class, if only for errno management.
 * Fairly recent errno implementations are per-thread, but even so one has to store away the errno
 * after every operation that affects it. This class is handy for the latter.
 *
 * This also wraps use of the syslog facility.
 */
class PosixWrapper {
  static bool needsInit;
protected:
  //added for libusb, extends error numbers in the negative direction.
  const char **alttext=nullptr;
  unsigned numalts=0;
public:
  /** for use with functions that return directly the codes that usually go into errno. */
  bool failure(int errcode);
  /** test posix function return value for "ok". log error code if not.
    this updates errornumber member on each call, but only emits a system log message if the errno has changed since the last time a message was sent.*/
public: //so that we can merge errors from functions called outside of the object which still use the same reporting mechanism
  bool failed(int zeroorminus1);
  /** syntactic sugar for !@see failed() */
  bool ok(int zeroorminus1){
    return !failed(zeroorminus1);
  }

  bool isOk()const noexcept{
    return errornumber==0;
  }

  /** set @param updatee with @param valorminus1, if valorminus1 is -1 then call failure(errno), return valorminus1>=0 */
//  bool okValue(int &updatee,int valorminus1);
  template <typename Integrish> bool okValue(Integrish &target,Integrish value){
    target=value;
    if(value==Integrish(~0UL)){//if magic 'all ones' value
      return ! failure(errno);//record and report on errno.
    } else {
      return true;//value deemed ok
    }
  }

public:
  /** ERRNO for last operation done using the extend object */
  int errornumber=0;
  /** logging noisiness for the (extended) object */
  int debug=0;
public:
  /** @param prefix is used for logging. */
  PosixWrapper(const char *prefix);
  Logger dbg;//todo: migrate logmsg use into dbg then kill the extra layer it requires.
  void logmsg(const char *fmt, ...);
  /** like strerror but with our stored value, not tied to errno */
  const char *errorText()const;
  /** replaces what was once 'failure' as that name works better for another purpose.
   * if @param passthrough is true then errno is recorded else errornumber is left to a previous value.
   * @returns passthrough.
 */
  bool setFailed(bool passthrough);
}; // class PosixWrapper

#endif // POSIXWRAPPER_H
