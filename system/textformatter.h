#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include "textpointer.h"
#include "charformatter.h"
#include "numberformatter.h"

#include "utf8.h" //isdigit
/** class for formatting values into a Text object. this replaces most of glib::ustring's extensions to std::string */
class TextFormatter : public Text {
  NumberFormat nf;
  CharFormatter body;
  /** tag of next argument to format+insert */
  int which = ~0;//weird value for debug, should always be written to before use by proper code.
  /** caller must compute largest possible expansion of args + format string then create this then call formatters*/
  TextFormatter(int presize);

  /** when we have no space the print is a dry run that computes instead of formats */
  int sizer = 0;
public:
  ~TextFormatter();
private:
  void substitute(TextKey stringy){
    if(this->length()>0) {
      body.remove(2);
    } else {
      sizer -= 2; //remove tag
      sizer += Cstr(stringy).length();
    }
//todo: insert string without changing body allocation.
  }

  //** compiler might have figured this out, but it is nice to able to breakpoint on this until we have proven the class */
  void substitute(Cstr stringy){
    substitute(stringy.c_str());
  }

  void substitute(double value){
    char widest[Zguard(nf.needs())];
    CharFormatter workspace(widest,sizeof(widest));
    if( !workspace.printNumber(value,nf.precision)) {//if failed to insert anything
      workspace.printChar('?');//replaces '%'
      workspace.printDigit(which);
    }
    substitute(workspace.internalBuffer());
  }

  /** templated printf:
   *  each argument is pulled out of the pack from left to right.
   *  if the argument is a format spec then we alter state and proceed.
   *  for arguments that hae a substitute method that will get called.
   *  Each substitue method eventually gots to substitue  (TextKey) which inserts a string.
   */
  template<typename ... Args> void next(const Args& ... args){
    ++which;
    if(sizeof ... (args) > 0) {//terminates tail recursion
      compose_item( args ...);
    }
  }

//argument spec inline
  template<typename ... Args> void compose_item( NumberFormat &item, const Args& ... args){
    nf = item;
    next(args ...);
  }

  template<typename NextArg, typename ... Args> void compose_item( NextArg&item, const Args& ... args){
    body.rewind();
    while(body.hasNext()) {
      char c = body.next();
      if(c == '%'&&body.hasNext()) {
        //todo: parseInt so that we canhave more than 10 args
        char d = body.next();
        if(d - '0' == which) { //splice in ref
          substitute(item);
          //by not returning here we allow for multiple substitutions of one argument.
        }
      }
    }
    next(args ...);
  } // compose_item

public:
  TextFormatter();
  /** @returns maximum number of chars it will take to show the given number */
  static unsigned estimateNumber(double value);
  /** @returns maximum number of chars it will take to show the given unicode char (given 1st byte of utf8) */
  static unsigned estimateSlashu(char unifirst);
  /** @returns maximum number of chars it will take to show the given string escaping with \u and \U */
  static unsigned estimateSlashu(TextKey value);


public:
  /** @returns composition of arguments using NumberFormatter rules */
  template<typename ... Args> static Text compose(TextKey format, const Args ... args){
    TextFormatter dryrun(0); //a zero size formatter computes required length via a dry run at formatting
    dryrun.compose(format,args ...);

    TextFormatter builder(Zguard(dryrun.sizer));
    builder.compose(format, args ...);
    return builder;
  }

}; // class TextFormatter

#endif // TEXTFORMATTER_H
