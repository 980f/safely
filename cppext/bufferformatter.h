#ifndef BUFFERFORMATTER_H
#define BUFFERFORMATTER_H

#include "charformatter.h"
#include "numberformat.h"
#include "halfopen.h"
#include "utf8.h" //isdigit
#include "cstr.h" //sane string

/** for args safe printing into a buffer.
 * This differs from TextFormatter class which will resize the buffer, this will just clip.
*/
class BufferFormatter {
  /** wraps Text for assembling string */
  CharFormatter body;
  /** stateful number formatting, an inline NF item applies to all higher indexed values */
  NumberFormat nf;
  /** tag of next argument to format+insert */
  unsigned which = BadIndex;//weird value for debug, should always be written to before use by proper code.
  /** bounds of replacement specification */
  Span spec;
public:
  BufferFormatter(const CharFormatter &other,TextKey format);
//  ~BufferFormatter();
private:
  bool insert(const char *stringy,unsigned length);

  void substitute(Cstr stringy);

  void substitute(Indexer<char> buf);

  void substitute(double value);

  /** templated printf:
   *  each argument is pulled out of the pack from left to right.
   *  if the argument is a format spec then we alter state and proceed.
   *  for arguments that have a substitute method that will get called.
   *  Each substitue method eventually calls substitute (TextKey) which inserts a string.
   */
  template<typename ... Args> void next(const Args& ... args){
    ++which;
    if(sizeof ... (args) > 0) {//terminates tail recursion
      compose_item( args ...);
    }
  }

  template<typename ... Args> void next(){
    //done
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
      if(c == '$'&&body.hasNext()) {
        spec.lowest=body.ordinal()-1;// -1 is to overwrite the '$'
        UTF8 d = body.next();
        if(d.isDigit()){//single digit simple spec
          if(d - '0' == which) { //splice in ref
            spec.highest=body.ordinal();
            substitute(item);//moves pointer into body, so an insertion can't loop to itself or earlier items.
            //by not returning here we allow for multiple substitutions of one argument.
          }
        }
        //todo: parseInt so that we can have more than 10 args

      }
    }
    next(args ...);
  } // compose_item

public:

  /** */
  template<typename ... Args> static void composeInto(CharFormatter target,TextKey format, const Args ... args){
    BufferFormatter worker(target,format); //a zero size formatter computes required length via a dry run at formatting
    worker.compose_item(args ...);
  }

}; // class TextFormatter



#endif // BUFFERFORMATTER_H
