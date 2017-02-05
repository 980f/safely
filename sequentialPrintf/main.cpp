#include <iostream>

using namespace std;

#include "char.h"
#include "cheaptricks.h"



class StreamPrintf {
  ostream &cout;
  //dummy writer, will pass a writer function into printer once it works well.
  template<typename Any> void write(Any &&c){
    cout << c;
  }

  /** ran out of arguments */
  bool PrintItem(unsigned ){
    return false;//ran off end of arg list
  }

  template<typename First,typename ... Args> bool PrintItem(unsigned which, const First first, const Args ... args){
    if(which==0) {
      write(first);
      return true;
    } else {
      return PrintItem(which - 1,args ...);
    }
  }

  bool parsingIndex = false;
  unsigned argIndex = ~0;//init to invalid

  bool parsingFormat = false;
  bool invertOption = false;
  bool keepFormat = false;
  unsigned formatValue = ~0;

  /** called after item has been printed */
void dropIndex(){
  parsingIndex=false;
  argIndex=~0;
}

/** called after parsing of format is done and any options have been applied */
void dropFormat(){
  parsingFormat = false;
  formatValue = ~0;
  invertOption = false;
  keepFormat=false;
}
/** called after 'have format field' has been recognized */
void startFormat(){
  parsingIndex = false;

  parsingFormat = true;
  formatValue = 0;

  invertOption=false;
  keepFormat=false;
}

bool appliedDigit(char c, unsigned &accumulator){
  if(Char(c).isDigit()) {
    accumulator *= 10;
    accumulator += c - '0';//primitive get digit
    return true;
  } else {
    return false;
  }
}

void missingField(){
  write('{');
  write(argIndex);//but formatting is long lost, this is an indicator of where the problem is so this is good enough.
  write('}');
}

public:
 StreamPrintf(ostream &ostr):cout(ostr){}

template<typename ... Args> void Printf(const char *fmt, const Args ... args){
  char c;
  while((c = *fmt++)) {
    if(parsingIndex) {
      if(appliedDigit(c,argIndex)){
        continue;
      }
      if(c==':') {//format tweak
        startFormat();
        continue;
      }
      if(c=='}') {
        if(!PrintItem(argIndex,args ...)) {
          missingField();
        }
        dropIndex();
        continue;
      }
      //else is bad format string, what do we do?
      missingField();
      dropIndex();
      //but proceed with trash char getting printed
    }
    if(parsingFormat) {
      if(appliedDigit(c,formatValue)){
        continue;
      }
      switch(c) {
        case '}'://not sure what we should do with formatValue, some default thing like width?
          if(formatValue){
            cout.width(formatValue);
          }
          if(!PrintItem(argIndex,args ...)) {
            missingField();
          }
          dropIndex();
          if(!keepFormat){
//            cout<<std::ios::reset(~0);//todo: only clear flags that our options might set, or track ones we have set. or save flags when we start formatParsing and write them back here.
          }
          dropFormat();
          break;
        case '!':
          //todo: do NOT reset stream after print
          keepFormat=true;
          break;
      case '-':
        invertOption = true;
        break;
      case 'w':
        cout.width(formatValue);
        break;
      case 'p':
        cout.precision(formatValue);
        break;
      default:
        //use 'c' as fill char:
        cout.fill(c);
        break;
      case 'l':  //left align
        break;
      case 'r':  //fight align
        break;
      case 'i':  //internal align
        break;
      case 'd':  //decimal
        cout << dec;
        break;
      case 'o':  //octal
        cout << oct;
        break;
      case 'b':  //binary
      case 'h':  //hex
        cout << hex;
        break;
      case 's':  //scientific
      case 'f':  //float
        //todo: showbase (0x prefix and the like)
        break;
      } // switch
      formatValue = 0;
      invertOption = 0;
      continue;
    }
    if(c=='{') {
      parsingIndex = true;
      argIndex = 0;
      continue;
    }
    write(c);
  }
} // Printf
};
int main(int, char *[]){
  StreamPrintf pf(cout);
  pf.Printf("One {0} Two {1:10} Three {2} and not {5}",1,2.3,"tree");
  cerr << endl;
  return 0;
}
