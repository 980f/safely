#ifndef STREAMPRINTF_H
#define STREAMPRINTF_H

#include "streamformatter.h"
///////////////////////
/// output stream variation
///
/// //because of templates, we must include some files that normally would only be in a cpp:
#include "index.h"  //isValid
#include "char.h"

class StreamPrintf:StreamFormatter {
  std::ostream&cout;
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
  StreamPrintf(std::ostream&stream);
  template<typename ... Args> void operator()(const char *fmt, const Args ... args){
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
        c=Char(c).slashee();
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

class StreamScanf:StreamFormatter {
  std::istream&cin;

  /** bytes that are not inside a format spec are to be checked for existence, and must match or we bail out*/
  bool verify(char c){
    char tocheck;
    cin>>tocheck;
    return c==tocheck;
  }


  void scan(int *c){
    int local=*c;
    cin >> local;
    *c=local;
  }

  template<typename Any> void scan(Any *c){
    Any local=*c;
    cin >> local;
    *c=local;
  }


  void scanMissingItem(){
    //not sure what to do ... the number converted will be wrong ... so the caller can debug it.
  }

  /** ran out of arguments. This is needed even if it never gets called. */
  void ScanItem(unsigned ){
    //return false;//ran off end of arg list
  }

  template<typename First,typename ... Args> void ScanItem(unsigned which, First &first, Args ... args){
    if(which==0) {
      scan(first);
    } else {
      ScanItem(which - 1,args ...);
    }
  }

public:
  StreamScanf(std::istream&stream);
  /** @returns the number of successfully parsed items. If that is less than the number given then the negative of that is returned.
   * an expert might be able to remove the need to put '7' in front of each argument, but at least this guy won't compile if you don't. */
  template<typename ... Args> int operator()(const char *fmt, Args ... args){
    char c;
    int numberScanned=0;
    beginParse();
    while((c = *fmt++)) {
      switch(applyItem(c)) {
      case DoItem:
        if(argIndex>= sizeof... (args)){
          scanMissingItem();
        } else {
          ScanItem(argIndex,args ...);
          ++numberScanned;
        }
        afterActing();
        break;
      case Escaped:
        c=Char(c).slashee();
        //#JOIN
      case Pass:
        if(!verify(c)){
          return -numberScanned;//which is OK for 0, this is a quantity not an index.
        }
        break;
      case FeedMe:
        //part of format field, nothing need be done.
        break;
      }
    }
    //if format pending then we have a bad trailing format spec, what shall we do?
    if(isValid(argIndex)){
      scanMissingItem();//even if item exists we shall not print it.
    }
    return numberScanned;
  } // Printf

};

#endif // STREAMPRINTF_H
