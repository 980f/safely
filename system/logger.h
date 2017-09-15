#ifndef LOGGER_H
#define LOGGER_H  "(C) 2017 Andrew Heilveil"

#include "stdarg.h" //for relaying arg packs through layers

#if LoggerManagement == 0
#include "chained.h"
#endif

/** a minimalist logging facade */
class Logger
#if LoggerManagement == 1
: public Chained<Logger> {
  static ChainedAnchor<Logger> root;
#else
{
#endif

public:
  /** must point to static text, is printed on the log before each message */
  const char *prefix;
  bool enabled;
  Logger(const char *location,bool enabled=true);
#if LoggerManagement == 1
  static void listLoggers(Logger &dbg);

  virtual
#endif
  ~Logger();
  /** makes usage look like a function */
  void operator() (const char *msg, ...);

  void varg(const char *fmt, va_list &args);
  void dumpStack(const char *prefix);
}; // class Logger

/** you must instantiate these two objects somewhere in your project */

/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;
/** a globally shared logger, for really egregious problems */
extern Logger wtf;

#endif // LOGGER_H
