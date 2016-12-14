/**************************************************************************
** Copyright (C) 2011 Rigaku (Applied Rigaku Technologies)
** created: 1/6/2012 2012
**************************************************************************/

#include <glibmm/shell.h>
#include "shell.h"
#include "logger.h"

using namespace sigc;
using namespace Glib;

/*
Changed implementation around 2013-08-01 to NOT set the working directory to the filesystem root, that is the most dangerous choice possible.
Any program invoked this way had better set absolute paths if it needs a specific path.
Upgrading this class to take a CWD would be nice.
*/

int Shell::run(const std::vector< std::string > &argv) {
  try {
    int exit_status(~0);//value is for debug
    spawn_sync("", argv, SPAWN_STDOUT_TO_DEV_NULL | SPAWN_STDERR_TO_DEV_NULL, sigc::slot< void >(), nullptr, nullptr, &exit_status);
    return exit_status;
  } catch(SpawnError e) {
    dbg("Shell::run failed for %s:%s",argv[0].c_str(),e.what().c_str());
    return ~0;
  }
}

Pid Shell::run_async(const std::vector< std::string > &argv, int *in, int *out, int *err) {  
  try {
    Pid pid(0);
    spawn_async_with_pipes("", argv, SpawnFlags(0), slot< void >(), &pid, in, out, err);
    return pid;
  } catch(SpawnError e) {
    dbg("Shell::run_async failed for %s:%s",argv[0].c_str(),e.what().c_str());
    return ~0;
  }
}
