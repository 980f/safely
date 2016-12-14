/**************************************************************************
** Copyright (C) 2011 Rigaku (Applied Rigaku Technologies)
** created: 1/6/2012 2012
**************************************************************************/

#ifndef SHELL_H
#define SHELL_H

#include <vector>
#include <string>
#include <glibmm/spawn.h>

class Shell {
public:
  Shell();
  static int run(const std::vector< std::string > &argv);
  static Glib::Pid run_async(const std::vector< std::string > &argv, int *in = nullptr, int *out = nullptr, int *err = nullptr);
};

#endif // SHELL_H
