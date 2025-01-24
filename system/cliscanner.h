/**
// Created by andyh on 1/22/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <type_traits>


class CliScanner {
public:
  CliScanner(int argc, char **argv) : argc{argc}, argv{argv} {}

  bool stillHas(unsigned needed) const {
    return argi + needed < argc; //test: seems off by one.
  }

  char *operator()() {
    return argv[argi++];
  }

  template<typename StringAssignable> bool operator>>(StringAssignable &target) {
    if (argi < argc) {
      if constexpr (std::is_same<StringAssignable, bool>::value) {
        auto arg = argv[argi++];
        target == strcasecmp("true", arg) == 0 || strcasecmp("1", arg) == 0;
      } else if constexpr (std::is_integral<StringAssignable>::value) {
        char *end;
        target = strtoll(argv[argi++], &end, 10);
        if (errno == ERANGE) {
          return false; //and leave errno as is
        }
        //todo: SI letters id end not trivial.
      } else { //type must have an operator=(char *);
        target = argv[argi++];
      }
      return true;
    } else if constexpr (std::is_floating_point<StringAssignable>::value) {
      char *end;
      target = strtod(argv[argi++], &end);
      //todo: test errno
      //todo: si letters?
      return true;
    } else {
      errno = EDOM; //todo: this might not be the traditional error code for "index out of range"
      return false;
    }
  }

private:
  const int argc;
  char **const argv;
  int argi = 0;
};
