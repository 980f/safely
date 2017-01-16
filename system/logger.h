//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
  Logger();
  /** must point to static text, is printed on the log before each message */
  const char *prefix;
  bool enabled;
  Logger(const char *location,bool enabled=true);

  virtual ~Logger();
  /** makes usage look like a function */
  void operator() (const char *msg, ...);

  void dumpStack(const char *prefix);
}; // class Logger


/** a globally shared logger, for when you are too lazy to create one for your context */
extern Logger dbg;
extern Logger wtf;

#endif // LOGGER_H
