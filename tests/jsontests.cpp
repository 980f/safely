#include "storejson.h"
#include "filer.h"
#include "textpointer.h"
#include "stopwatch.h"

static const char*jsontests[] = {
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


void printNode(unsigned tab,Storable &node){
  bool pretty=Index(tab).isValid();
  if(pretty){
    putchar('\n');
    for(unsigned tabs = tab; tabs-->0; ) {
      printf("  ");
    }
  }
  if(node.name.empty()) {
    //just print tabs
  } else {
    printf("\"%s\" : ",node.name.c_str());
  }
  switch (node.getType()) {
  case Storable::Wad:
    printf("{");
    for(auto list(node.kinder()); list.hasNext(); ) {
      Storable & it(list.next());
      printNode(pretty?tab + 1:BadIndex,it);
      if(list.hasNext()) {
        putchar(',');
      } else {
        if(pretty){
          putchar('\n');
          for(int tabs = tab; tabs-->0; ) {
            printf("  ");
          }
        }
        putchar('}');
      }
    }
    break;
  case Storable::Numerical:
    printf("%g ",node.getNumber<double>());
    break;
  case Storable::Uncertain:
  case Storable::NotKnown:
    printf("%s ",node.image().c_str());
    break;
  case Storable::Textual:
    if(node.image().empty()){
      putchar('"');
      //else printf converts null ptr to (null)
      putchar('"');
    } else {
      printf("\"%s\" ",node.image().c_str());
    }
    break;
  } // switch  
  fflush(stdout);
} // switch

void testJson(const char *block,unsigned size){
  dbg("testJson: testing: %s",block);

  Indexer<const char> loaded(block,size);
  Storable *root = nullptr;
  StoredJSONparser parser(loaded,root);
  StopWatch perftimer;
  bool retval = parser.parse(root);
  perftimer.stop();
  dbg("JsonParse: after %g ms returned: %d  nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000.0, retval,parser.s.totalNodes, parser.s.totalScalar, parser.s.maxDepth.extremum);

  if(root) {
    printNode(1,*root);
    putchar('\n');
      fflush(stdout);
  }
}   // testJson

#include "testabstractjsonparser.h"

void testAbstractly(const char *block,unsigned size){
  dbg("Jabstract: testing: %s",block);
  Indexer<const char> loaded(block,size);

  TAJParser parser(loaded);
//  dbg("\nJsonPreParse: nodes:%u  scalars:%u depth:%u",parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);
  StopWatch perftimer;
  parser.parse();

  dbg("JsonParse: after %g ms nodes:%u  scalars:%u depth:%u",perftimer.elapsed()*1000, parser.stats.totalNodes, parser.stats.totalScalar, parser.stats.maxDepth.extremum);

  if(parser.core.root) {
    printNode(1,*parser.core.root);
    putchar('\n');
    fflush(stdout);  //to show up in debugger ASAP.
  }
}   // testJson


void testJ(unsigned which,bool newer){
  if(Index(which).isValid()) {
    Cstr test(jsontests[which]);
    if(newer){
      testAbstractly(test.c_str(),test.length());
    } else {
      testJson(test.c_str(),test.length());
    }
  } else {
    for(int which = countof(jsontests); which-->0; ) {      
      testJ(which,newer);
    }
  }
}   // testJ
