#ifndef SHELL_H
#define SHELL_H "(C) 2017 Andrew Heilveil"

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
