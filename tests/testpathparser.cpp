#include "testpathparser.h"
#include "logger.h"

TestPathParser::TestPathParser(){

}

void TestPathParser::parse(const char *rawpath,char sep){
  rules=PathParser::parseInto(path,rawpath,sep);
}

const char *testdata[]={
  "/this/is/a/path",
  "/rootdir/",
  "dirlike/",
  "/rooted",
  "simple"

};

void TestPathParser::run(unsigned which){
  if(Index(which).in(countof(testdata))){
    TestPathParser ppt;
    ppt.parse(testdata[which]);
    dbg("Parsed:<%s> %c got pre:%u post:%u pieces:%u",testdata[which],ppt.rules.slash,ppt.rules.before,ppt.rules.after);
  }

}