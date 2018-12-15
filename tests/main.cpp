#include <iostream>

#include "stdio.h"
#include <initializer_list>  //for inline test data sets

#include "safestr.h"
#include "logger.h"

#include "unistd.h" //getcwd

#include "storable.h"
#include "numericalvalue.h" //union + type enum
#include "stddef.h"
/** test harness for qtcreator debugger helper*/
void testPrettyPrinter(unsigned which){
  switch(which) {
  case BadIndex:
    for(which=4;which-->0;){
      testPrettyPrinter(which);
    }
    break;
  case 3: {
    Index eye;
    eye=3;
    eye=BadIndex;
    eye++;
    dbg("printing eye so that compiler doesn't drop it before we can see it with the debugger: %u",eye.raw);
  } break;
  case 2: {
    Text forlabelling;
    forlabelling = "hi dad!";

  }
  break;
  case 1: {
    NumericalValue en;
    en.setto(12.34);

    dbg("is:%u for %u, storage:%u len:%u",offsetof(NumericalValue,is),sizeof(NumericalValue::is),offsetof(NumericalValue,storage),sizeof(NumericalValue::storage));

    en.changeInto(NumericalValue::Counting);
    en.setto(43);
    en.changeInto(NumericalValue::Whole);
    en.setto(-1234);
    en.changeInto(NumericalValue::Counting);
    en.changeInto(NumericalValue::Whole);
    en.changeInto(NumericalValue::Truthy);
    en.setto(false);
    en.changeInto(NumericalValue::Floating);
    en.setto(3.14159);
  }
  break;
  case 0: {
    NumericalValue en;
    en.setto(12.34);

    Storable node("grandma");
    node.setType(Storable::Textual);
    node.setType(Storable::Numerical);
    node.setNumber(en);
    Storable &mom=node.child("mommy");
    Storable &dad=node.child("daddy");
    for(unsigned ci=3;ci-->0;){
      mom.addChild("girls").presize(3,Storable::Numerical);
      dad.addChild("boys").presize(ci,Storable::Numerical);
    }

  }
  break;

  } // switch

} // testPrettyPrinter

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
  MaxDoubleFinder maxish;
  MinDoubleFinder minish;
  Extremer<double,true,true> lastish;

  unsigned which = 0;
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

  CharFormatter buffer(bigenough,sizeof (bigenough));
  //nonzero number less than 400 with sigfic 0 printed all 000's
  buffer.clearUnused();//the printers don't presume to know where the end of the string is.
  for(int ipow = 4; ipow-->-4; ) {
    buffer.rewind();
    double d = dpow10(ipow);
    auto ok = buffer.printNumber(d,0);
    buffer.next() = 0;
    dbg("CF[10^%d]->%d:%s",ipow,ok,bigenough);
  }
  buffer.rewind();
  BufferFormatter::composeInto(buffer,"One $1",1984);
  dbg("\nShould be <One 1984>:<%s>",bigenough);
} // testBufferFormatter

#include "fildes.h"
#include <functional>
void showSizes(){
  dbg("Size of fildes: %d",sizeof (Fildes));
  std::function<void()> *nullfunctor;
  dbg("Size of minimal functor: %d",sizeof (nullfunctor));
}

extern void testJ(unsigned which);
#include "unicodetester.h"
#include "numberformatter.h"
#include "testpathparser.h"
#include "filereadertester.h"
#include "filewritertester.h"
#include "application.h"

//rpi i2c
#include "SSD1306.h"

SSD1306 hat({128,64,true,12});

int main(int argc, char *argv[]){
  Text cwd(getcwd(nullptr,0));//we use Text class because it will free what getcwd allocated. Not so critical unless we are using this program to look for memory leaks
                              // in the functions it tests.
  dbg("Working directory is: %s",cwd.c_str());
//  dbg("Static loggers list:");
//  Logger::listLoggers(dbg);
  Application::writepid("tests.pid");
  while(argc-->0) {
    const char*tes = argv[argc];
    dbg("%d: %s",argc,tes);
    char group = (*tes++);
    unsigned which = atoi(tes);
    switch(group) {
    case '%':
      testPrettyPrinter(which);
      break;
    case 'z':
      showSizes();
      break;
    case 'w': {
      FileWriterTester().run(which);
    } break;
    case 'f': {
      FileReaderTester().run(which);
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
    } // switch
  }
  dbg("tests completed \n");
  return 0;
} // main
