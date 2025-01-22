#pragma once
#define LOGGER_H  "(C) 2017 Andrew Heilveil"

#include <cstdarg> //for relaying arg packs through layers

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

  /** output newline and flush the file */
  void flushline() const;
  //used for legacy in PosixWrapper
  void varg(const char *fmt, va_list &args) const;
  //try to produce a stack trace, not as reliable as Java.
  void dumpStack(const char *prefix);
  //RIAA for merging messages:
  struct Combiner {
    Logger &logger;
    bool wasCombining;
    Combiner(Logger &logger) : logger(logger){
      wasCombining = logger.combining;
      logger.combining = true;
    }
    //NB: ensure that Move constructor works or this class will be costly at runtime, issuing a useless flushline.
    ~Combiner(){
      if(!wasCombining) {
        logger.flushline();
        logger.combining = false;
      }
    }

  };

  Combiner beMerging() {
    return Combiner(*this);
  }
public:
  /** 1st implementation coupled to a json file for enabling/disbling by logger name.
   * Logger construction and destruction call these members if a global one exists. */
  struct Manager {
    virtual void onCreation(Logger &logger) = 0;
    virtual void onDestruction(Logger &logger) = 0;
    virtual ~Manager() = default;
  };
  static Manager *manager;
}; // class Logger

// you must instantiate these two objects somewhere in your project:
/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;
/** a globally shared logger, for really egregious problems */
#define Safely_Have_Wtf
extern Logger wtf;

#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())

//typical allocation of a managed logger, deals with concern of warning non-POD static
#define SafeLogger(loggerName,deflevel) \
  __attribute__((init_priority(202)))  \
  static Logger loggerName( #loggerName, deflevel )
