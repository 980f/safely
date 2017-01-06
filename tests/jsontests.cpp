#include "storejson.h"
#include "filer.h"
#include "textpointer.h"

static const char*jsontests[] = {
  "braced1:\"{1}\"",
  "embedcomma:\"com,ma\"",
  "embedcolonv:\"col:on\"",
  "\"embed:colonn\":vacuum",

  "group:{f1:{s1:2},f2:vf2}",

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
  dbg("\ntestJson: testing: %s",block);

  Indexer<const char> loaded(block,size);
  Storable *root = nullptr;
  StoredJSONparser parser(loaded,root);
  bool retval = parser.parse(root);

  dbg("\n JsonParse: returned: %d  nodes:%u  scalars:%u depth:%u",retval,parser.totalNodes, parser.totalScalar, parser.maxDepth.extremum);

  if(root) {
    printNode(~0,*root);
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
