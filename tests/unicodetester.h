#ifndef UNICODETESTER_H
#define UNICODETESTER_H

#include "index.h"
#include "textpointer.h"

class UnicodeTester
{
public:
  UnicodeTester();

  static bool run(unsigned which=BadIndex);
  static Text escape(const char *testdata);
  static Text descape(const char *testdata);
};
#endif // UNICODETESTER_H
