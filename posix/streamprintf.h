#ifndef STREAMPRINTF_H
#define STREAMPRINTF_H

#include <iostream>

//because of templates, we must include this now:
#include "index.h"

class StreamFormatter {
protected:
  std::ostream &cout;

  /** for controlling the scope of application of formatting. */
  struct StreamState {
    std::ios_base::fmtflags flags;
    std::streamsize width;
    std::streamsize precision;
    char fill;
    void record(std::ostream &cout);
    void restore(std::ostream &cout);
  };
  //initial implementation is one save and restore per print session:
  StreamState pushed;

//format string parse states and values:
  bool parsingIndex;
  unsigned argIndex;

  bool parsingFormat;
  unsigned formatValue;

  bool slashed;//next char is fill char
  bool invertOption;
  bool keepFormat;
  //format parse state transition functions:
  /** resets all state to 'no format spec seen', records stream state for later optional restoration. */
  void beginParse();

  /** called after item has been printed, to avert bugs */
  void dropIndex();

/** called after parsing of format is done and any options have been applied */
  void dropFormat();

/** called after 'have format field' has been recognized */
  void startFormat();

/** @returns whether c is a digit,and if so then it has applied it to the accumulator  */
  bool appliedDigit(char c, unsigned &accumulator);
  /** call when the format commands has been fully acted upon, IE after responding to 'DoItem' */
  void afterActing();
  /** begin parsing the item number*/
  void startIndex();
  /** format value has been used, avert applying it to another feature */
  void clearFormatValue();

protected://now for the API:
  enum Action {
    FeedMe, //feed parser more characters
    Pass,   //process the char, for printing: output it; for scanf compare it
    Escaped,//the char was an escape char, e.g. if c==n then emit a newline.
    DoItem, //end of format field, act upon it.
  };
  /** inspects format character @param c and @returns what to do */
  Action applyItem(char c);

// the following did not work as the ostream inherited virtually from the base class that actually had the methods needed:
//  typedef std::streamsize (std::ostream::*Attributor)(std::streamsize);
//  void applyFormat(Attributor func);
  enum FormatValue {Widthly, Precisely};
  void applyFormat(FormatValue whichly);

  /** attach to a stream */
  StreamFormatter(std::ostream &ostr);
}; // class StreamPrintf


class StreamPrintf:StreamFormatter {
  template<typename Any> void write(Any &&c){
    cout << c;
  }

  void printMissingItem();

  /** ran out of arguments. This is needed even if it never gets called. */
  void PrintItem(unsigned ){
    //return false;//ran off end of arg list
  }

  template<typename First,typename ... Args> void PrintItem(unsigned which, const First first, const Args ... args){
    if(which==0) {
      write(first);
    } else {
      PrintItem(which - 1,args ...);
    }
  }

public:
  StreamPrintf(std::ostream&cout);
  template<typename ... Args> void Printf(const char *fmt, const Args ... args){
    char c;
    beginParse();
    while((c = *fmt++)) {
      switch(applyItem(c)) {
      case DoItem:
        if(argIndex>= sizeof... (args)){
          printMissingItem();
        } else {
          PrintItem(argIndex,args ...);
        }
        afterActing();
        break;
      case Escaped:
        if(c=='n'){//just doing one for proof of principle, will import c-escape stuff and do the usualy suspects
          c='\n';
        }
        //#JOIN
      case Pass:
        write(c);
        break;
      case FeedMe:
        //part of format field, nothing need be done.
        break;
      }
    }
    //if format pending then we have a bad trailing format spec, what shall we do?
    if(isValid(argIndex)){
      printMissingItem();//even if item exists we shall not print it.
    }
  } // Printf


};

#endif // STREAMPRINTF_H
