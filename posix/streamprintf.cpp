#include "streamprintf.h"

void StreamPrintf::printMissingItem(){
  write('{');
  write(argIndex);  //but formatting is long lost, this is an indicator of where the problem is so this is good enough.
  write('}');
}

StreamPrintf::StreamPrintf(std::ostream &cout):
  StreamFormatter (cout),
  cout(cout){
  //we retain a copy as the more derived type of stream while passing it as a base to the baser class.
}

StreamScanf::StreamScanf(std::istream &stream):
  StreamFormatter(stream),
  cin(stream)
{
  //#nada
}
