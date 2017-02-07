#ifndef STREAMPRINTF_H
#define STREAMPRINTF_H

#include <iostream>

//because of templates, we must include this now:
#include "index.h"

class StreamPrintf {
  std::ostream &cout;
  std::ios_base::fmtflags pushedFlags;

  void beginParse();

  template<typename Any> void write(double &&c){
//    std::cerr<<"Member:"<<&cout<<" cout:"<<&std::cout<<" this:"<<this<<std::endl;
    cout.flush();
    std::cerr << '<' << cout.flags() << '>';
    cout << c;
  }


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

  bool parsingIndex;
  unsigned argIndex;//init to invalid

  bool parsingFormat;
  bool invertOption;
  bool keepFormat;
  unsigned formatValue;

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

  void afterPrinting();

public:
  /** attach to a stream */
  StreamPrintf(std::ostream &ostr);


  template<typename ... Args> void Printf(const char *fmt, const Args ... args){
    char c;
    beginParse();
    while((c = *fmt++)) {
      if(printNow(c)) {
        if(!PrintItem(argIndex,args ...)) {
          missingField();
        }
        afterPrinting();
      }
    }
  } // Printf

  void startIndex();
  void clearFormatValue();
protected:
// the following did not work as the ostream inherited virtually from the base class that actually had the methods needed:
//  typedef std::streamsize (std::ostream::*Attributor)(std::streamsize);
//  void applyFormat(Attributor func);
  enum FormatValue {Widthly, Precisely};
  void applyFormat(FormatValue whichly);
}; // class StreamPrintf

#endif // STREAMPRINTF_H
