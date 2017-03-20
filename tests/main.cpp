#include <iostream>

#include "stdio.h"
#include <initializer_list>  //for inline test data sets

#include "safestr.h"
#include "logger.h"

#include "unistd.h" //getcwd

SafeStr<14> fortnight;

//simply compiling the following is a demanding thing:
Indexer<SafeStr<10>> pressure;

#include "watchable.h"

static Watchable<int> demonic;

void demonWatcher(int newvalue){
  dbg("\ndemonic: %d",newvalue);
}

void justOnce(int newvalue){
  dbg("\njustOnce: %d",newvalue);
}

void justOnceLater(int newvalue){
  dbg("\njustOnceLater: %d",newvalue);
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
    dbg("\nDeleteOnExitTestData.%s",TextKey(message));
  }

  static void testme(){
    DeleteOnExitTestData &doe(*new DeleteOnExitTestData("I'm dying here!"));
    DeleteOnExit<DeleteOnExitTestData> dor(doe);
    dbg("this should be followed with another printout");
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
  dbg("\nMax %g at %u",maxish.extremum,maxish.location);
  dbg("\nMin %g at %u",minish.extremum,minish.location);
  dbg("\nLastish %g at %u",lastish.extremum,lastish.location);

} // extremely


#include "bufferformatter.h"

void testBufferFormatter(){
  char bigenough[200];

  CharFormatter buffer(bigenough);
  BufferFormatter::composeInto(buffer,"One $1",1984);
  dbg("\nShould be <One 1984>:<%s>",bigenough);
}


extern void testJ(unsigned which);
#include "unicodetester.h"
#include "numberformatter.h"
#include "testpathparser.h"
#include "filereadertester.h"
#include "filewritertester.h"
int main(int argc, char *argv[]){
  Text cwd(getcwd(nullptr,0));//we use Text class because it will free what getcwd allocated. Not so critical unless we are using this program to look for memory leaks in the functions it tests.
  dbg("Working directory is: %s",cwd.c_str());
  dbg("Static loggers list:");
  Logger::listLoggers(dbg);
  while(argc-->0) {
    const char*tes=argv[argc];
    dbg("%d: %s",argc,tes);
    char group=(*tes++);
    unsigned which=atoi(tes);
    switch(group){
    case 'w':{
      FileWriterTester fwt;
      fwt.run(which);
    } break;
    case 'f':{
      FileReaderTester frt;
      frt.run(which);
    } break;
    case 'b'://buffer formatting
      testBufferFormatter();
      break;
    case 'j': //json tests
      testJ(which);//newer implementation
      break;
    case 'p'://pathparser tests
      TestPathParser::run(which);
      break;
    case 'u':
      UnicodeTester::run();
      break;
    case 'e':
      extremely();
      break;
    case 'n':
      dbg("NumberFormatter: %s",NumberFormatter::makeNumber(14.5).c_str());
      break;
    case 'x':
      DeleteOnExitTestData::testme();
      testdemonic();
    {
      int coedata(42);
      coe(coedata);
      dbg("coe: %d should be 0",coedata);
    }
      break;
    }
  }
  dbg("tests completed \n");
  return 0;
} // main
