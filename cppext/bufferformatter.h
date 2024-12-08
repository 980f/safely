#ifndef BUFFERFORMATTER_H
#define BUFFERFORMATTER_H "(C) Andrew L. Heilveil, 2017"

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
  BufferFormatter(const Indexer<u8> &other, TextKey format);
  BufferFormatter(CharFormatter &buffer,TextKey format);
  BufferFormatter(char *raw, unsigned sizeofraw,TextKey format);
  BufferFormatter(unsigned char *raw, unsigned sizeofraw,TextKey format);

  /** prepare for new format run. @returns this */
  BufferFormatter &setFormat(TextKey format);
//  ~BufferFormatter();
private:
  bool insert(const char *stringy,unsigned length);

  void substitute(Cstr stringy);
  void substitute(TextKey stringy);

  void substitute(Indexer<char> buf);
  void substitute(CharFormatter buf);

  void substitute(double value);
  void substitute(u64 value);

  void substitute(u32 value);
  void substitute(u16 value);
  void substitute(u8 value);

  void substitute(s32 value);
  void substitute(s16 value);
  void substitute(s8 value);

  void substitute(bool value);

  void substitute(char value);

  /** compiler insists we have this, needed in case the format string references this non-printable item.*/
  void substitute( const NumberFormat &item);


  /** each argument is pulled out of the pack from left to right.
   *  if the argument is a format spec then we alter state and proceed.
   *  for arguments that have a substitute method that will get called.
   *  Each substitute method eventually calls substitute (TextKey) which inserts a string.
   */
  template<typename ... Args> void next(const Args& ... args){
    ++which;
    if(sizeof ... (args) > 0) {//terminates tail recursion
      compose_item( args ...);
    }
  }

/** probably unused since we did the size check in next(...args) else
* terminate varags processing */
  template<typename ... Args> void next(){
    //done
  }

//if it is a number format then record it and apply to following items, no substition takes place..
  template<typename ... Args> void compose_item(const NumberFormat &item, const Args& ... args){
    nf = item;
    next(args ...);
  }

  template<typename NextArg, typename ... Args> void compose_item(const NextArg&item, const Args& ... args){
    body.rewind();
    while(body.hasNext()) {
      char c = body.next();
      if(c==0){//short the loop, saves time.
        break;
      }
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
        //todo:1 parseInt so that we can have more than 10 args
      }
    }
    next(args ...);
  } // compose_item

public:

  /** */
  template<typename ... Args> static void composeInto(CharFormatter &target,TextKey format, const Args ... args){
    BufferFormatter worker(target,format);
    worker.compose_item(args ...);
    target.skip(worker.body.used());
  }

  template<typename ... Args> void print( const Args ... args){
    compose_item(args ...);
  }

};



#endif // BUFFERFORMATTER_H
