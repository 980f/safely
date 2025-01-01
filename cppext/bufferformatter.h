#pragma once //"(C) Andrew L. Heilveil, 2017"

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
  BufferFormatter(const Indexer<uint8_t> &other, TextKey format);
  BufferFormatter(CharFormatter &buffer,TextKey format);
  BufferFormatter(char *raw, unsigned sizeofraw,TextKey format);
  BufferFormatter(unsigned char *raw, unsigned sizeofraw,TextKey format);

  /** prepare for new format run. @returns this.
    normal use is to call print() write after this.  */
  BufferFormatter &setFormat(TextKey format);
//  ~BufferFormatter();
private:
  bool insert(const char *stringy,unsigned length);

  void substitute(Cstr stringy);
  void substitute(TextKey stringy);

  void substitute(Indexer<char> buf);
  void substitute(CharFormatter buf);

  void substitute(double value);
  void substitute(uint64_t value);

  void substitute(uint32_t value);
  void substitute(uint16_t value);
  void substitute(uint8_t value);

  void substitute(int32_t value);
  void substitute(int16_t value);
  void substitute(int8_t value);

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

/** if it is a number format then record it and apply to following items, no substiuttion takes place.. */
  template<typename ... Args> void compose_item(const NumberFormat &item, const Args& ... args){
    nf = item;
    next(args ...);
  }

  template<typename NextArg, typename ... Args> void compose_item(const NextArg&item, const Args& ... args){
    body.rewind();
    while(body.hasNext()) {
      char c = body.next();
      if(c==0){//short the loop, saves debug time.
        break;
      }
      if(c == '$'&&body.hasNext()) {
        spec.lowest=body.ordinal()-1;// -1 is to overwrite the '$'
        UTF8 d = body.next();
        if(d.isDigit()){//single digit simple spec
          if(d - '0' == which) { //then splice in ref
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

  /** as a service rather than an object:
    Modifies content of @param target according to @param format and var args list. */
  template<typename ... Args> static void composeInto(CharFormatter &target,TextKey format, const Args ... args){
    BufferFormatter worker(target,format);
    worker.compose_item(args ...);
    target.skip(worker.body.used());
  }

  /** adds more args to an instance whose formatter had higher numbered field references than were processed in prior call.
    Which pretty much makes no sense unless you have just called setFormat after rewinding the buffer. */
  template<typename ... Args> void print(const Args ... args){
    compose_item(args ...);
  }

};


template<unsigned maxsize> struct FormattedMessage {
  char body[maxsize + 1];
  const char *format;
  BufferFormatter buff;
  FormattedMessage(const char *format) :
      format(format), buff(body, maxsize + 1, format) {
  }

  template<typename... Args> void print(const Args... args) {
    buff.setFormat(format);
    buff.print(args ...);
  }

};
