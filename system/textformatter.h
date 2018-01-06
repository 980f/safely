#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H "(C) Andrew L. Heilveil, 2017"

#include "textpointer.h"
#include "charformatter.h"
#include "numberformatter.h"

#include "utf8.h" //isdigit


/** class for formatting values into a Text object. this replaces most of glib::ustring's extensions to std::string
 * compared to other string class implemenations this takes pain to realloc only when absolutely necessary. It does not do fancy memory management like copy-on-write,,
 * the fanciest thing it does is predict what a bunch of concatenation will require before attempting that concatenation, allocating just what is needed.
(first pass it overallocates numbers, until we are sure we can perfectly predict the size of numbers we overallocate, still allocating just once, and have some extra leftover bytes).


*/
class TextFormatter : public Text {
public://4diagnostics
  /** wraps Text for assembling string */
  CharFormatter body;
  /** stateful number formatting, an inline NF item applies to all higher indexed values */
  NumberFormat nf;
protected:
  /** where a terminator should be after formatting */
  unsigned termloc=BadIndex;
  unsigned dataend=BadIndex;
  /** whether we are computing size of final string or assembling it */
  bool sizing=true;
  /** tag of next argument to format+insert */
  unsigned which = BadIndex;//weird value for debug, should always be written to before use by proper code.
  /** when we have no space the print is a dry run that computes instead of formats */
  unsigned sizer = 0;
//got tired of passing this around, nice to have it handy
  unsigned width=BadLength;
  //width of field being replaced
  unsigned substWidth=BadLength;
private:
  TextFormatter()=delete;
public:
  ~TextFormatter();
private:

  void substitute(CharFormatter buf);
  void substitute(Cstr stringy);
  void substitute(TextKey stringy);
  void substitute(double value);

  void substitute(u64 value);
  void substitute(u32 value);
  void substitute(u16 value);
  void substitute(u8 value);
  //todo: signed ints as well
  void substitute(bool value);


  /** compiler insists we have this, needed in case the format string references this non-printable item.*/
  void substitute( const NumberFormat &item);

  bool onSizingCompleted();

  /** templated printf:
   *  each argument is pulled out of the pack from left to right.
   *  if the argument is a format spec then we alter state and proceed.
   *  for arguments that have a substitute method that will get called.
   *  Each substitute method eventually inserts a string.
   */
  template<typename NextArg, typename ... Args> void compose_item( NextArg&item, const Args& ... args){
    body.rewind();
    bool slashed=false;
    while(body.hasNext()&&body.ordinal()<dataend) {
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
          substWidth=2;//dollar and digit
          width=BadLength;//for debug
          substitute(item);
          //by not returning here we allow for multiple substitutions of one argument.
        }
      }
    }
    //inlined to reduce stack depth, we only had two copies:    next(args...);
      ++which;
      compose_item( args ...);
  } // compose_item

  template<typename ... Args> void compose_item( const NumberFormat &item, const Args& ... args){
    substitute(item);
    //inlined to reduce stack depth, we only had two copies:    next(args...);
    ++which;
    compose_item( args ...);
  }

  template<typename ... Args> void compose_item( ){
    //# here is where we can do any post processing such as freeing of no longer needed caching of converted items.
  }

  bool openSpace();
  void reclaimWaste(const CharFormatter &workspace);

  CharFormatter makeWorkspace();
  bool processing();
  void onFailure(CharFormatter workspace);

public:
  TextFormatter(TextKey mf);

  /** applys args, @returns whether it actually did so. This object *is* Text so it has c_str() etc. if needed. */
  template<typename ... Args> bool apply(const Args ... args){
    which=0;
    sizing=true;
    termloc=sizer=body.allocated();
    compose_item(args ...);
    if(onSizingCompleted()){
      which=0;
      compose_item(args ...);
      if(body.canContain(termloc)){//if we did a perfect job then the buffer is perfect as is.
        body[termloc]=0;//clip trash from overallocating workspace for numbers.
      }
      return true;
    } else {
      //couldn't allocate a buffer or the string to build is empty.
      return false;
    }
  }

  Indexer<u8> asBytes();

  /** @returns composition of arguments using NumberFormatter rules */
  template<typename ... Args> static Text compose(TextKey format, const Args ... args){
    TextFormatter worker(format);
    worker.apply(args...);
    return worker;
  }

}; // class TextFormatter

#endif // TEXTFORMATTER_H
