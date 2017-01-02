#include <iostream>

#include "stdio.h"

#include <initializer_list>  //for inline test data sets

#include "safestr.h"

SafeStr<14> fortnight;

//simply compiling the following is a demanding thing:
Indexer<SafeStr<10>> pressure;

#include "watchable.h"

static Watchable<int> demonic;

void demonWatcher(int newvalue){
  printf("\ndemonic: %d\n",newvalue);
}

void justOnce(int newvalue){
  printf("\njustOnce: %d\n",newvalue);
}

void justOnceLater(int newvalue){
  printf("\njustOnceLater: %d\n",newvalue);
}

#include "runoncenthtime.h"
void testdemonic(){

  demonic.onAnyChange(RunOnceSlot<int>::makeInstance(&justOnce));
  demonic.onAnyChange(&demonWatcher);
  //run once on second change
  demonic.onAnyChange(RunOnceNthTime<int>::makeInstance(&justOnceLater,2));

  demonic = 0;//should be no change
  //should print justOnce: 17
  demonic = 17;//should print 17
  demonic = 0;//should print 0
  //should print justOnceLater:0
  demonic = 22;//should print 0
  //nothing else should print.
} // testdemonic

#include "cheaptricks.h"
void coe(int &shouldclear){
  ClearOnExit<int> raii(shouldclear);
  shouldclear *= 5;
}

#include "textpointer.h"
class DeleteOnExitTestData {
  Text message;
public:
  DeleteOnExitTestData(TextKey msg) : message(msg){
  }

  ~DeleteOnExitTestData(){
    printf("\nDeleteOnExitTestData.%s",TextKey(message));
  }

  static void testme(){
    DeleteOnExitTestData &doe(*new DeleteOnExitTestData("I'm dying here!"));
    DeleteOnReturn<DeleteOnExitTestData> dor(doe);
    printf("\nthis should be followed with another printout");
  }

}; // class DeleteOnExitTestData


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

  DeleteOnExitTestData::testme();

  testdemonic();

  int coedata(42);
  coe(coedata);
  printf("\ncoe: %d should be 0",coedata);

  extremely();
  printf("\ntests completed \n");
  return 0;
} // main