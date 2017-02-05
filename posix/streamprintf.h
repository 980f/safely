#ifndef STREAMPRINTF_H
#define STREAMPRINTF_H

#include <iostream>


class StreamPrintf {
  std::ostream &cout;
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
  void dropIndex();

/** called after parsing of format is done and any options have been applied */
  void dropFormat();

/** called after 'have format field' has been recognized */
  void startFormat();

  bool appliedDigit(char c, unsigned &accumulator);

  void missingField();

  /** inspects format character @param c and @returns whether an item should be printed. */
  bool printNow(char c); // printNow

  void afterPrinting(){
    dropIndex();
    if(!keepFormat) {
//            cout<<std::ios::reset(~0);//todo: only clear flags that our options might set, or track ones we have set. or save flags when we start formatParsing and
// write them back here.
    }
    dropFormat();
  }

public:
  /** attach to a stream */
  StreamPrintf(std::ostream &ostr);

  template<typename ... Args> void Printf(const char *fmt, const Args ... args){
    char c;
    while((c = *fmt++)) {
      if(printNow(c)) {
        if(formatValue) {
          cout.width(formatValue);
        }
        if(!PrintItem(argIndex,args ...)) {
          missingField();
        }
        afterPrinting();

      } // Printf
    }
  } // Printf

  void startIndex();
}; // class StreamPrintf

#endif // STREAMPRINTF_H
