//(C) 2017 Andrew Heilveil
#include "logger.h"
#include "eztypes.h"
#include "textkey.h"
#include "stdarg.h"

/** you must implement these two functions somewhere in your project */
extern void logmessage(const char *prefix,const char *msg,va_list &args);
extern void dumpStack(const char *prefix);

Logger::Manager *Logger::manager=nullptr;
//LoadJSON,DBG,Filewriter,IFT,WTF,FSA,FileReader,FileWriter
Logger::Logger(const char *location,bool enabled):prefix(location),enabled(enabled){
  if(manager){
    manager->onCreation(*this);
  }
}


Logger::~Logger(){
  if(manager){
    manager->onDestruction(*this);
  }
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

