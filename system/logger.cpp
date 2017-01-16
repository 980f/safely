//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#include "logger.h"
#include "eztypes.h"
#include "textkey.h"
#include "stdarg.h"


Logger::Logger() : prefix(0){
  //ctor
}

Logger::Logger(const char *location,bool enabled):prefix(location),enabled(enabled){
  //#nada
}

Logger::~Logger(){
  //dtor
}

extern void logmessage(const char *prefix,const char *msg,va_list &args);

void Logger::operator() (const char *msg, ...){
  if(enabled){
    va_list args;
    va_start(args, msg);
    logmessage(prefix, msg, args);
    va_end(args);
  }
}

extern void dumpStack(const char *prefix);
void Logger::dumpStack(const char *prefix){
  ::dumpStack(prefix);
}

