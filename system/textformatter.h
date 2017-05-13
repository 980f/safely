#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include "textpointer.h"
#include "charformatter.h"
#include "numberformatter.h"

#include "utf8.h" //isdigit


/** class for formatting values into a Text object. this replaces most of glib::ustring's extensions to std::string
 * compared to other string class implemenations this takes pain to realloc only when absolutely necessary. It does not do fancy memory management like copy-on-write,,
 * the fanciest thing it does is predict what a bunch of concatenation will require before attempting that concatenation, allocating just what is needed.

*/
class TextFormatter : public Text {
  /** wraps format string, gets rewound a lot during sizing */
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
  TextFormatter()=delete;
public:
  ~TextFormatter();
private:
  void substitute(TextKey stringy);

  //** compiler might have figured this out, but it is nice to able to breakpoint on this until we have proven the class */
  void substitute(Cstr stringy);

  /** the primary substituter, all others defer to this */
  void substitute(CharFormatter buf);

  void substitute(double value);

  /** needed in case format string references this non-printable item. Might use that instead of counting to reuse these.
   * could also not bump 'which' when one is encountered, but that has its own confusions.
*/
  void substitute( const NumberFormat &item);

  bool onSizingCompleted();

  /** templated printf:
   *  each argument is pulled out of the pack from left to right.
   *  if the argument is a format spec then we alter state and proceed.
   *  for arguments that have a substitute method that will get called.
   *  Each substitute method eventually inserts a string.
   */
  template<typename ... Args> void next(const Args& ... args){
    ++which;
    compose_item( args ...);
  }


  template<typename NextArg, typename ... Args> void compose_item( NextArg&item, const Args& ... args){
    body.rewind();
    bool slashed=false;
    while(body.hasNext()) {
      char c = body.next();
      if(flagged(slashed)){
        continue;
      }
      if(c=='\\'){
        slashed=true;
        continue;
      }
      if(c == '$'&&body.hasNext()) {
        //todo: parseInt so that we can have more than 10 args
        char d = body.next();
        if(d - '0' == which) { //splice in ref
          substitute(item);
          //by not returning here we allow for multiple substitutions of one argument.
        }
      }
    }
    next(args...);
  } // compose_item

  template<typename ... Args> void compose_item( ){
    //# here is where we can do any post processing such as freeing of no longer needed caching of converted items.
  }

//  template<typename ... Args> void compose_item( NumberFormat &item, const Args& ... args){
//    substitute(item);
//    next(args...);
//  }

  template<typename ... Args> void compose_item( const NumberFormat &item, const Args& ... args){
    substitute(item);
    next(args...);
  }

public:

  /** @returns maximum number of chars it will take to show the given number */
  static unsigned estimateNumber(double value);
  /** @returns maximum number of chars it will take to show the given unicode char (given 1st byte of utf8) */
  static unsigned estimateSlashu(char unifirst);
  /** @returns maximum number of chars it will take to show the given string escaping with \u and \U */
  static unsigned estimateSlashu(TextKey value);


public:
  template<typename ... Args> bool apply(const Args ... args){
    which=0;
    sizing=true;
    sizer=format.allocated();//instead of adding one for each simple char we will subtract for the substititution tags.
    compose_item(args ...);
    if(onSizingCompleted()){
      which=0;
      compose_item(args ...);
      return true;
    } else {
      //couldn't allocate a buffer or the string to build is empty.
      return false;
    }
  }

  /** @returns composition of arguments using NumberFormatter rules */
  template<typename ... Args> static Text compose(TextKey format, const Args ... args){
    TextFormatter worker(format);
    worker.apply(args...);
    return worker;
  }

}; // class TextFormatter

#endif // TEXTFORMATTER_H
