#include <iostream>

#include "vargs.h"
#include "stdio.h"

//#include <cstdio>


class PosixTester {
public:
  void varger(const char *fmt,...){
    PFlist args(fmt);
    vprintf(fmt,args);
  }

  int testVargs(){

  }

  int testFiler(){
    return -1;
  }
};

int main(int argc, char *argv[])
{
  PosixTester tester;
  tester.testFiler();
  return 0;
}
