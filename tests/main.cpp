#include <iostream>

#include "stdio.h"

#include <initializer_list>  //for inline test data sets

#include "safestr.h"

SafeStr<14> fortnight;

Indexer<SafeStr<10>> pressure;

#include "watchable.h"

Watchable<int> demonic;

void demonWatcher(int newvalue){
  printf("\ndemonic: %d\n",newvalue);
}

#include "cheaptricks.h"
void coe(int &shouldclear){
  ClearOnExit<int> raii(shouldclear);
  shouldclear *= 5;
}

#include "extremer.h"
void extremely(){
  Extremer<double> maxish;
  Extremer<double> minish(true);
//  auto testset=/*{1.0,4.2,3.7,8.9,-2.71828,9.5,3.4}*/;
  int which = 0;
  for(auto x:{1.0,4.2,3.7,8.9,-2.71828,9.5,3.4}) {
    minish.inspect(which,x);
    maxish.inspect(which++,x);
  }
  printf("\nMax %g at %u",maxish.extremum,maxish.location);
  printf("\nMin %g at %u",minish.extremum,minish.location);
} // extremely

int main(int argc, char *argv[]){
  while(argc-->0) {
    printf("\n%d: %s",argc,argv[argc]);
  }
  demonic.onAnyChange(&demonWatcher);
  int coedata(42);
  coe(coedata);
  printf("\ncoe: %d should be 0",coedata);
  extremely();
  printf("\ntests completed \n");
  return 0;
} // main
