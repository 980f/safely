
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
    return argi + needed < argc;
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
        const bool range_error = errno == ERANGE;
        if (range_error) {
          return false; //and leave errno as is
        }
        //todo: SI letters
      } else { //type must have an operator=(char *);
        target = argv[argi++];
      }
      return true;
    } else {
      errno = EDOM;
      return false;
    }
  }


private:
  const int argc;
  char ** const argv;
  int argi = 0;
};
