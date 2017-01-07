//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#include "logger.h"
#include "eztypes.h"
#include <stdio.h>
#include <stdarg.h>
#include "unistd.h"
#include "stdlib.h"
#include <signal.h>
#include <execinfo.h>
#include <iostream>
#include <math.h>

//stderr or stdout.
#define stout stderr

Logger dbg("",true);

Logger::Logger() : prefix(0){
  //ctor
}

Logger::Logger(const char *location,bool enabled):prefix(location),enabled(enabled){
  //#nada
}

Logger::~Logger(){
  //dtor
}

void show(FILE * stdf,const char *prefix,const char *msg,va_list &args){
  if(prefix) {
    fputs(prefix,stdf);
    fputs("::",stdf);
  }
  vfprintf(stdf, msg, args);
  fputc('\n', stdf);
  fflush(stdf); //else debug messages from just before croaking aren't seen.
}

void Logger::operator() (const char *msg, ...){
  if(enabled){
    va_list args;
    va_start(args, msg);
    show(stout, prefix, msg, args);
    va_end(args);
  }
}

/** a signal handler */
void fatalHandler(int signal, siginfo_t *signalInfo, void */*data*/){
  bool fatal = false;
  if(SIGSEGV == signal) {
    fatal = true;
    fprintf(stderr, "Fault accessing memory address %p\n", signalInfo->si_addr);
  } else if(SIGILL == signal) {
    fatal = true;
    fprintf(stderr, "Fault executing instruction\n");
  } else if(SIGUSR1 == signal) {
    fprintf(stderr, "Debug signal\n");
  }
  void *stack[100];
  int stackSize = backtrace(stack, 100);
  // Recover the address where the exception happened
#if defined(__i386__)
  ucontext_t *context = static_cast< ucontext_t* >(data);
  void *addr = reinterpret_cast< void* >(context->uc_mcontext.gregs[REG_EIP]);
#elif defined(__ARM_EABI__)
  ucontext_t *context = static_cast< ucontext_t* >(data);
  void *addr = reinterpret_cast< void* >(context->uc_mcontext.arm_pc);
#else
  void *addr = reinterpret_cast< void* >(0xdeadbeef);
#endif
  if(addr == stack[2]) { // Some libc's have the correct address already in the 3rd frame
    backtrace_symbols_fd(&stack[2], stackSize - 2, stderr->_fileno);
  } else { // Otherwise the 2nd frame is the libc's signal handler, we overwrite that
    stack[1] = addr;
    backtrace_symbols_fd(&stack[1], stackSize - 1, stderr->_fileno);
  }
  fdatasync(stderr->_fileno);
  if(fatal) {
    _exit(-1);
  }
} // fatalHandler

void dumpStack(const char *prefix){
  dbg("StackTrace requested by %s",prefix);
  raise(SIGUSR1);
}

void Logger::ClassInit(bool trapSignals){
  if(trapSignals) {
    struct sigaction sa;
    sa.sa_sigaction = &fatalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGSEGV, &sa, nullptr);
//SIGPIPE's were screwing up our graceful restart on socat disconnect.
//    Note that gdb turns the signal back on but you can defeat that with the gdb command: handle SIGPIPE nostop
    sigignore(SIGPIPE);
  }
} // Logger::ClassInit

void wtf(const char *msg, ...){
  va_list args;
  va_start(args, msg);
  show(stout, ";P",msg, args);
  va_end(args);
}
