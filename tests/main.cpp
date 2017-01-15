#include <iostream>

#include "stdio.h"

#include <initializer_list>  //for inline test data sets

#include "safestr.h"
#include "logger.h"

SafeStr<14> fortnight;

//simply compiling the following is a demanding thing:
Indexer<SafeStr<10>> pressure;

#include "watchable.h"

static Watchable<int> demonic;

void demonWatcher(int newvalue){
  printf("\ndemonic: %d",newvalue);
}

void justOnce(int newvalue){
  printf("\njustOnce: %d",newvalue);
}

void justOnceLater(int newvalue){
  printf("\njustOnceLater: %d",newvalue);
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
  MaxDouble maxish;
  MinDouble minish;
  Extremer<double,true,true> lastish;

  int which = 0;
  for(auto x:{1.0,4.2,-2.71828,3.7,8.9,-2.71828,9.5,3.4}) {
    minish.inspect(which,x);
    lastish.inspect(which,x);
    maxish.inspect(which++,x);
  }
  printf("\nMax %g at %u",maxish.extremum,maxish.location);
  printf("\nMin %g at %u",minish.extremum,minish.location);
  printf("\nLastish %g at %u",lastish.extremum,lastish.location);

} // extremely

extern void testJ(unsigned which,bool newer);
#include "unicodetester.h"
#include "numberformatter.h"
#include "testpathparser.h"

int main(int argc, char *argv[]){
  while(argc-->0) {
    dbg("%d: %s",argc,argv[argc]);
  }
  fflush(stdout);//without this flush the text above injected itself into testJ's output ...

  TestPathParser::run(0);
  exit(0);

  extremely();

  Text puff=NumberFormatter::makeNumber(14.5);
  printf("\nNumberFormatter: %s",puff.c_str());

  UnicodeTester::run();
//  exit(0);

  testJ(BadIndex,true);
  testJ(BadIndex,false);

  DeleteOnExitTestData::testme();

  testdemonic();

  int coedata(42);
  coe(coedata);
  printf("\ncoe: %d should be 0",coedata);

  printf("\ntests completed \n");
  return 0;
} // main
