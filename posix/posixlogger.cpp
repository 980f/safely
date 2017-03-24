#include "posixlogger.h"
#include <stdarg.h>
#include "unistd.h" //fdatasync _exit
#include <signal.h>
#include <execinfo.h> //backtrace
#include "cstr.h" //nonTrivial

//implements what system/logger.h externs:
#if LoggerManagement == 1
ChainedAnchor<Logger> Logger::root(nullptr,false);//most will be either static (never delete) or auto (delete by compiler on exit of scope)
//todo: return logger by name
//todo: how do we access the list without a gui?
#endif

Logger dbg("DBG");
Logger wtf("WTF");

#define stdf stderr

void logmessage(const char *prefix,const char *msg,va_list &args){
  if(nonTrivial(prefix)) {
    fputs(prefix,stdf);
    fputs("::",stdf);
  }
  vfprintf(stdf, msg, args);
  fputc('\n', stdf);
  fflush(stdf); //else debug messages from just before croaking aren't seen.
}

void dumpStack(const char *prefix){
  dbg("StackTrace requested by %s",prefix);
  raise(SIGUSR1);
}


/** a signal handler */
void fatalHandler(int signal, siginfo_t *signalInfo, void *data){//#don't hide 'data', some platforms access it.
  bool fatal = false;
  if(SIGSEGV == signal) {
    fatal = true;
    fprintf(stderr, "Fault accessing memory address %p\n", signalInfo->si_addr);
  } else if(SIGILL == signal) {
    fatal = true;
    fprintf(stderr, "Fault executing instruction\n");
  } else if(SIGUSR1 == signal) {
    fprintf(stderr, "Stack trace requested\n");
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


void PosixLoggerInit(bool trapSignals){
  if(trapSignals) {
    struct sigaction sa;
    sa.sa_sigaction = &fatalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGSEGV, &sa, nullptr);
//SIGPIPE's interfere with graceful restart on socat disconnect.
//    Note that gdb turns the signal back on but you can defeat that with the gdb command: handle SIGPIPE nostop
    sigignore(SIGPIPE);
  }
} // Logger::ClassInit

