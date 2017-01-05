
#include "storejson.h"
#include "filer.h"
#include "textpointer.h"

const char*jsontests[]={

  "group:{first:lonely,}",
  "group:{first:lonely,second:twofer}",
  "group:{first:1}",

  "\"namely\":\"value less\"",
  "namely:\"value less\"",
  "namely:valueless",
  "namely : valueless",
  " namely : valueless ",
};

void testJson(const char *block,unsigned size){
  Indexer<const char> loaded(block,size);
  Storable *root=nullptr;
  StoredJSONparser parser(loaded,root);
  bool retval=parser.parse(root);

  printf("\n JsonParse: returned: %d",retval);
  printf("\t nodes:%u", parser.totalNodes);
  printf("\t scalars:%u", parser.totalScalar);
  printf("\t depth:%u", parser.maxDepth.extremum);
  printf("\t dangling:%u", parser.nested);

}


void testJ(unsigned which){
  if(Index(which).isValid()){
    Cstr test(jsontests[which]);
    testJson(test.c_str(),test.length());
  } else {
    for(int which=countof(jsontests);which-->0;){
      Cstr test(jsontests[which]);
      testJson(test.c_str(),test.length());
    }
  }
}
