
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



void printNode(unsigned tab,Storable &node){
  putchar('\n');
  for(int tabs=tab;tabs-->0;){
    puts("  ");
  }
  if(node.name.empty()){
    //just print tabs
  } else {
    puts(node.name.c_str());
    puts(" : ");
  }
  switch (node.getType()) {
  case Storable::Wad:
    puts("{");
    for(auto list(node.kinder());list.hasNext();){
      Storable & it(list.next());
      printNode(tab+1,it);
      puts( list.hasNext()?" ,":" }");
    }
    break;
 case Storable::Numerical:
    printf("%g",node.getNumber<double>());
    break;
  case Storable::Uncertain:
  case Storable::NotKnown:
  case Storable::Textual:
    puts(node.image().c_str());
    break;
  }

}

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
  if(root){
    printNode(0,*root);
  }
  fflush(stdin);//to show up in debugger before app terminates.
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
