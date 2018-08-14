#include "charformatter.h"
#include "testnumberparsing.h"
#include "logger.h"

#include "cstr.h"
TestNumberParsing::TestNumberParsing(const char *testdata){
  Cstr inp(testdata);
  CharFormatter p(inp.violated(),inp.length());
  do{
    double arg = p.parseDouble();
    if(!isSignal(arg)) {   //#isnormal inconveniently excludes zero.
      dbg("found: %g, ending with %c",arg,p.next());
    } else {
      dbg("signal: %g",arg);
    }
  } while(p.hasNext());
}
