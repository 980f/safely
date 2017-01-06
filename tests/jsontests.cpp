#include "storejson.h"
#include "filer.h"
#include "textpointer.h"

static const char*jsontests[] = {

  "group:{first:lonely,}",
  "group:{first:lonely,second:twofer}",
  "group:{first:1}",

  "\"n 5\":\"v5 l5\"",
  "n4:\"v4 b4\"",
  "n3:v3",
  "n2 : v2",
  " n1 : v1 ",
};


void printNode(unsigned tab,Storable &node){
  putchar('\n');
  for(int tabs = tab; tabs-->0; ) {
    printf("  ");
  }
  if(node.name.empty()) {
    //just print tabs
  } else {
    printf("%s : ",node.name.c_str());
  }
  switch (node.getType()) {
  case Storable::Wad:
    printf("{");
    for(auto list(node.kinder()); list.hasNext(); ) {
      Storable & it(list.next());
      printNode(tab + 1,it);
      if(list.hasNext()) {
        putchar(',');
      } else {
        putchar('\n');
        for(int tabs = tab - 1; tabs-->0; ) {
          printf("  ");
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
  case Storable::Textual:
    printf("%s ",node.image().c_str());
    break;
  } // switch
  fflush(stdout);
} // switch

void testJson(const char *block,unsigned size){
  dbg("\ntestJson: testing: %s",block);

  Indexer<const char> loaded(block,size);
  Storable *root = nullptr;
  StoredJSONparser parser(loaded,root);
  bool retval = parser.parse(root);

  dbg("\n JsonParse: returned: %d  nodes:%u  scalars:%u depth:%u",retval,parser.totalNodes, parser.totalScalar, parser.maxDepth.extremum);

  if(root) {
    printNode(0,*root);
    fflush(stdout);  //to show up in debugger before app terminates.
  }
}   // testJson

void testJ(unsigned which){
  if(Index(which).isValid()) {
    Cstr test(jsontests[which]);
    testJson(test.c_str(),test.length());
  } else {
    for(int which = countof(jsontests); which-->0; ) {
      Cstr test(jsontests[which]);
      testJson(test.c_str(),test.length());
    }
  }
}   // testJ
