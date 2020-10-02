#ifndef LOGGER_H
#define LOGGER_H  "(C) 2017 Andrew Heilveil"

#include "stdarg.h" //for relaying arg packs through layers

/** a minimalist logging facade */
class Logger {
public:
  /** must point to static text, is printed on the log before each message */
  const char *prefix;
  bool enabled;
  /** used by combiner*/
  bool combining = false;
  Logger(const char *location,bool enabled = true);
  ~Logger();
  /** makes usage look like a function */
  void operator() (const char *msg, ...);

  /** output newine and flush the file */
  void flushline();
  //used for legacy in PosixWrapper
  void varg(const char *fmt, va_list &args);
  void dumpStack(const char *prefix);
  //RIAA for merging messages:
  struct Combiner {
    Logger &logger;
    bool wasCombining;
    Combiner(Logger &logger) : logger(logger){
      wasCombining = logger.combining;
      logger.combining = true;
    }

    ~Combiner(){
      if(!wasCombining) {
        logger.flushline();
        logger.combining = false;
      }
    }

  };

public:
  struct Manager {
    virtual void onCreation(Logger &logger) = 0;
    virtual void onDestruction(Logger &logger) = 0;
    virtual ~Manager() = default;
  };
  static Manager *manager;
}; // class Logger

/** you must instantiate these two objects somewhere in your project */

/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;
/** a globally shared logger, for really egregious problems */
//trying to reconcile with integer version, versus this textly one: extern Logger wtf;
#define wtf(...) dbg(  __VA_ARGS__ )

#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())

//typical allocation of a managed logger
#define SafeLogger(loggerName,deflevel) \
  __attribute__((init_priority(202)))  \
  static Logger loggerName( #loggerName, deflevel )

#endif // LOGGER_H
