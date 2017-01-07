//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
  /** trapping signals is made conditional in case the application has its own handler.*/
  static void ClassInit(bool trapSignals = false);
  Logger();
  /** must point to static text, is printed on the log before each message */
  const char *prefix;

  Logger(const char *location);

  virtual ~Logger();
  /** makes usage look like a function */
  void operator() (const char *msg, ...);
}; // class Logger

/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;

/** call this when you are ignoring what might be a serious problem */
extern void wtf(const char *msg, ...);

/** call this when you want to know how you got to someplace where @see wtf() was worth calling.
 *  The implementation emits a signal SIGUSR1 so if your app uses that you will have to alter the implementation in logger.cpp */
void dumpStack(const char *prefix);

/** how to record an error from glib that you are ignoring. Useful in catch phrases. */
#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())

#endif // LOGGER_H
