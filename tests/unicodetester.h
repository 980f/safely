#ifndef UNICODETESTER_H
#define UNICODETESTER_H

#include "index.h"

class UnicodeTester
{
public:
  UnicodeTester();

  static bool run(unsigned which=BadIndex);
  static void escape(const char *testdata);
};
#endif // UNICODETESTER_H
