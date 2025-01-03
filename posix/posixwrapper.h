#pragma once
#define POSIXWRAPPER_H "(C) Andrew L. Heilveil 2017-2018,2024"

#include <cerrno> //ignore unused warning as we templated a function that needs it and every using class will refer to it (errornumber member)
#include "logger.h"
/**
 * handy things for wrapping a POSIX C API with a C++ class, if only for errno management.
 * Fairly recent errno implementations are per-thread, but even so one has to store away the errno
 * after every operation that affects it. This class is handy for the latter.
 *
 * This also wraps use of the syslog facility.
 */
class PosixWrapper {
protected:
  //libusbwrapper extends error numbers in the negative direction.
  const char **alttext = nullptr;
  unsigned numalts = 0;

public:
  /** for use with functions that return directly the codes that usually go into errno. */
  bool failure(int errcode);

  /** test posix function return value for "ok". log error code if not.
   *  this updates errornumber member on each call, but only emits a system log message if the errno has changed since the last time a message was sent.*/
public: //so that we can merge errors from functions called outside of the object which still use the same reporting mechanism
  bool failed(int zeroorminus1);

  /** syntactic sugar for !@see failed() */
  bool ok(int zeroorminus1) {
    return !failed(zeroorminus1);
  }

  bool isOk() const noexcept {
    return errornumber == 0;
  }

  /** @returns whether last error was just some variant of 'try again later' */
  bool isWaiting() const;

  /** set @param target with @param value, if value is -1 (all ones, ~0) then call failure(errno), @returns whether value!=~0. */
  template<typename Integrish, typename Other> bool okValue(Integrish &target, Other value) {
    target = static_cast<Integrish>(value);
    return !setFailed(value == Integrish(~0ULL)); // tilde operator is unduly restricted by compiler :(
  }

public:
  /** ERRNO for last operation done using the extend object */
  int errornumber = 0;
  /** ignore resource temporarily unavailable error. users sets this when that error is to be stifled from the log stream. */
  bool stifleEAGAIN = true;

public:
  /** @param prefix is used for logging. */
  PosixWrapper(const char *prefix);

  Logger dbg;

  /** like @see strerror() but with our stored value, not tied to errno */
  const char *errorText() const;

  /** replaces what was once 'failure' as that name works better for another purpose.
   * if @param passthrough is true then errno is recorded else errornumber is left to a previous value.
   * @returns passthrough.
   */
  bool setFailed(bool passthrough);
}; // class PosixWrapper

