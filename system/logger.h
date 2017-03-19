#ifndef LOGGER_H
#define LOGGER_H

/** a minimalist logging facade */
class Logger {
public:
  /** must point to static text, is printed on the log before each message */
  const char *prefix;
  bool enabled;
  Logger(const char *location,bool enabled=true);

  virtual ~Logger()=default;
  /** makes usage look like a function */
  void operator() (const char *msg, ...);

  void dumpStack(const char *prefix);
}; // class Logger

/** you must instantiate these two objects somewhere in your project */

/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;
/** a globally shared logger, for really egregious problems */
extern Logger wtf;

#endif // LOGGER_H
