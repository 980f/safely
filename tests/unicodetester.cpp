#include "unicodetester.h"
//#include "eztypes.h"
#include "utf8text.h"
#include "stdio.h"
#include "logger.h"

Text UnicodeTester::escape(const char *testdata)
{
  Cstr wrapped(testdata);
  if(wrapped.empty()){
    return Text("empty intput");
  }
  dbg("\nEscape:<%s> ->",testdata);
  Text probate(Utf8Text::encode(wrapped));
//  dbg("returned: %p/%p",&probate,probate.c_str());
  dbg("<%s>",probate.c_str());
  return probate;
}


Text UnicodeTester::descape(const char *testdata){
  Cstr wrapped(testdata);
  if(wrapped.empty()){
    return Text("empty intput");
  }
  dbg("\nrecode:<%s> ->",testdata);
  Text probate(Utf8Text::decode(wrapped));
//  dbg("returned: %p/%p",&probate,probate.c_str());
  dbg("<%s>",probate.c_str());
  return probate;
}


const char * unitests[]={
  "nothing",
  "\\x39OK\\x20spaced",
  "\\x39OK\x20spaced",
  "\\x39OK",
  "\uC0B0",  //some han character, 3 sections
  "\U0001f060",  //emoticon 6:5 domino
};

bool UnicodeTester::run(unsigned which){
  if(Index(which).in(countof(unitests))){
    descape(escape(unitests[which]));
  } else {
    for(which=countof(unitests);which-->0;){
      descape( escape(unitests[which]));
    }
  }
  return  true;
}
