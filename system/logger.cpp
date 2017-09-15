//(C) 2017 Andrew Heilveil
#include "logger.h"
#include "eztypes.h"
#include "textkey.h"
#include "stdarg.h"

/** you must implement these two functions somewhere in your project */
extern void logmessage(const char *prefix,const char *msg,va_list &args);
extern void dumpStack(const char *prefix);

Logger::Logger(const char *location,bool enabled):prefix(location),enabled(enabled){
#if LoggerManagement == 1
  root.append(this);
#endif
}

#if LoggerManagement == 1
void Logger::listLoggers(Logger &dbg){
  Chained<Logger> *scan=Logger::root.root;
  while(scan->peer){
    scan=scan->peer;
    Logger *log=dynamic_cast<Logger *>(scan);
    dbg("Logger.%s=%d",log->prefix,log->enabled);
  }
}
#endif

Logger::~Logger(){
#if LoggerManagement == 1
  root.remove(this);
#endif
}

void Logger::operator() (const char *fmt, ...){
  if(enabled){
    va_list args;
    va_start(args, fmt);
    logmessage(prefix, fmt, args);
    va_end(args);
  }
}

void Logger::varg(const char *fmt, va_list &args){
  if(enabled){
    logmessage(prefix, fmt, args);
  }
}

void Logger::dumpStack(const char *prefix){
  ::dumpStack(prefix);
}

