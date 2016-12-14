//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
  /** trapping signals conditional as nexqc has the same functionality embedded at the moment.*/
  static void ClassInit(bool trapSignals=false);
  Logger();
  const char *prefix;
  virtual ~Logger();
  void operator() (const char *msg, ...);
};

extern Logger dbg;

extern void wtf(const char *msg, ...);
void dumpStack(const char *prefix);

#define IgnoreGlib(err) dbg("%s ignoring %s",__PRETTY_FUNCTION__, err.what().c_str())

#endif // LOGGER_H
