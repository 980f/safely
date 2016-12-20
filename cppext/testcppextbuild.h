#ifndef TESTCPPEXTBUILD_H
#define TESTCPPEXTBUILD_H

#include "stdio.h"

#include <initializer_list>  //for inline test data sets

#include "safestr.h"

SafeStr<14> fortnight;

Indexer<SafeStr<10>> pressure;

#include "changemonitored.h"

Watched<int> demonic;

#include "cheaptricks.h"
void usefless(){
  int fart;
  ClearOnExit<int>farter(fart);
  fart=5;
}

#include "extremer.h"
void extremely(){
  Extremer<double> maxish;
  auto testset={1.0,4.2,3.7,8.9,9.5,3.4};
  int which=0;
  for(auto x:testset){
    maxish.inspect(which++,x);
  }
  printf("Max %g at %u",maxish.extremum,maxish.location);
}

class TestCppExtBuild
{
public:
  TestCppExtBuild();
};

#endif // TESTCPPEXTBUILD_H
