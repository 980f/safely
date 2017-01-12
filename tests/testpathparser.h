#ifndef TESTPATHPARSER_H
#define TESTPATHPARSER_H

#include "pathparser.h"

class TestPathParser {
public:
  PathParser::Rules rules;
  SegmentedName path;

  TestPathParser();
  void parse(const char *rawpath, char sep='/');
  Text pack();

  static void run(unsigned which=BadIndex);
};

#endif // TESTPATHPARSER_H
