//(C)2017 Andrew Heilveil
#include <glibmm/shell.h>
#include "shell.h"
#include "logger.h"


int Shell::run(const std::vector< std::string > &argv) {
  try {
    int exit_status(~0);//value is for debug
    spawn_sync("", argv, SPAWN_STDOUT_TO_DEV_NULL | SPAWN_STDERR_TO_DEV_NULL, sigc::slot< void >(), nullptr, nullptr, &exit_status);
    return exit_status;
  } catch(Glib::SpawnError e) {
    dbg("Shell::run failed for %s:%s",argv[0].c_str(),e.what().c_str());
    return ~0;
  }
}

Glib::Pid Shell::run_async(const std::vector< std::string > &argv, int *in, int *out, int *err) {  
  try {
    Glib::Pid pid(0);
    spawn_async_with_pipes("", argv, SpawnFlags(0), slot< void >(), &pid, in, out, err);
    return pid;
  } catch(Glib::SpawnError e) {
    dbg("Shell::run_async failed for %s:%s",argv[0].c_str(),e.what().c_str());
    return ~0;
  }
}
