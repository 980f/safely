#include "storejson.h"
#include "filer.h"
#include "textpointer.h"
#include "stopwatch.h"

#include <jsonfile.h>

static const char*jsontests[] = {
  "{array:[one,{two:blue}]}",
  "{array:[one,two]}",
  "{struct:{one,two}}",
  "braced1:\"{1}\"",
  "embedcomma:\"com,ma\"",
  "embedcolonv:\"col:on\"",
  "\"embed:colonn\":vacuum",

  "group:{f1:{s1:1,s2:2},f2:vf2}",

  "group:{first:lonely,}",                 //trailing comma in group
  "group:{first:lonely,second:twofer}",
  "group:{first:1}",

  "\"n 5\":\"v5 l5\"",
  "n4:\"v4 b4\"",
  "n3:v3",
  "n2 : v2",
  " n1 : v1 ",
  "emptyval:\"\"",
};



void testJ(unsigned which){
  if(Index(which).isValid()) {
    Cstr test(jsontests[which]);

    auto block=test.c_str();
    auto size=test.length();

    Indexer<char> loaded(test.violated(),size);
    dbg("StoreJSON: testing: %s",block);

    StoreJsonParser parser(loaded);

    StopWatch perftimer;
    parser.parse();
    dbg("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
//////
    if(parser.core.root) {
      Fildes stout("log");
      stout.preopened(stdout->_fileno,false);
      JsonFile printer(*parser.core.root);
      printer.printOn(stout,0,true);
      putchar('\n');
      fflush(stdout);  //to show up in debugger ASAP.
    }
  } else {
    for(unsigned which = countof(jsontests); which-->0; ) {
      testJ(which);
    }
  }
}   // testJ
