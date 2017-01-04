#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include "textpointer.h"
#include "charformatter.h"
#include "numberformatter.h"

#include "utf8.h" //isdigit


/** class for formatting values into a Text object. this replaces most of glib::ustring's extensions to std::string
 * compared to other string class implemenations this takes pain to realloc only when absolutely necessary. It does not do fancy memory management liek copy-on-write,,
 * the fanciest thing it does is predict what a bunch of concatenation will require before attempting that concatenation, allocating just what is needed.

*/
class TextFormatter : public Text {
  /** wraps format string, gets rewound a lot */
  CharScanner format;
  /** wraps Text for assembling string */
  CharFormatter body;
  /** stateful number formatting, an inline NF item applies to all higher indexed values */
  NumberFormat nf;
  /** whether we are computing size of final string or assembling it */
  bool sizing=true;
  /** tag of next argument to format+insert */
  unsigned which = BadIndex;//weird value for debug, should always be written to before use by proper code.
  /** when we have no space the print is a dry run that computes instead of formats */
  unsigned sizer = 0;

  struct Piece {
    // how many times reference in the string, usually 0 or 1
    unsigned refcount;
    // how many bytes it needed.
    unsigned length;
  };

//todo: count and malloc  Piece *lengthCache=nullptr;
  Piece lengthCache[10];//so long as we only can handle single digit place indicators we can do a fixed allocation here.

  struct Insertion {
    int which;
    //offset from start of final string where the insertion goes.
    int position;
  };
  //we allocate this by summing the refcounts of the lengthCache items.
  Insertion *inserts=nullptr;

  TextFormatter(TextKey mf);
private:
  TextFormatter();
public:
  ~TextFormatter();
private:
  void substitute(TextKey stringy){
    CharScanner risky=CharScanner::infer(Cstr::violate(stringy));
    risky.dump();
    if(sizing) {
      sizer -= 2; //remove tag
      sizer += risky.allocated()-1;
    } else {
//      body.remove(2);
      //todo: insert string without changing body allocation.
    }
  }

  //** compiler might have figured this out, but it is nice to able to breakpoint on this until we have proven the class */
  void substitute(Cstr stringy){
    substitute(CharFormatter::infer(stringy.violated()));
  }

  /** the primary substituter, all others defer to this */
  void substitute(CharFormatter buf){
    if(sizing){
      sizer-=2;
      sizer+=buf.used();
    } else {
      //shove data up
      //point to '$'
      body.rewind(2);
      //overlay
      body.appendUsed(buf);
    }
  }

  void substitute(double value){
    char widest[Zguard(nf.needs())];
    CharFormatter workspace(widest,sizeof(widest));
    if( !workspace.printNumber(value,nf.precision)) {//if failed to insert anything
      workspace.printChar('?');//replaces '%'
      workspace.printDigit(which);
    }
    substitute(workspace.used());
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

//if it is a number format then record it and apply to following items, no substition takes place..
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

  /** @returns maximum number of chars it will take to show the given number */
  static unsigned estimateNumber(double value);
  /** @returns maximum number of chars it will take to show the given unicode char (given 1st byte of utf8) */
  static unsigned estimateSlashu(char unifirst);
  /** @returns maximum number of chars it will take to show the given string escaping with \u and \U */
  static unsigned estimateSlashu(TextKey value);


public:
  /** @returns composition of arguments using NumberFormatter rules */
  template<typename ... Args> static Text compose(TextKey format, const Args ... args){
    TextFormatter worker(format); //a zero size formatter computes required length via a dry run at formatting
    worker.sizing=true;
    worker.compose(format,args ...);
    worker.sizing=false;
    worker.compose(format, args ...);
    return Text(worker.body.internalBuffer());
  }

}; // class TextFormatter

#endif // TEXTFORMATTER_H
