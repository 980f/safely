#ifndef BUFFER_H
#define BUFFER_H

#include "eztypes.h"
#include "minimath.h"
#include "sequence.h"
#include "ordinator.h"

/**
 *  This class is used to prevent buffer overflows.
 *  Instances are passed a pointer to a buffer and its length and provide pointer-like syntax for modifying and accessing said
 * buffer, but won't increment the pointer outside the allocated range.
 *  This class does not CONTAIN the data, it is a pointer into data allocated elsewhere.
 *
 *  There are two major modalities of use, writing and reading.
 *
 *  Writing starts with the pointer at zero and stuff is added until done or the pointer is at the end of allocation.
 *  One then uses the copy constructor to create a reader, with the pointer at zero and its allocation where the writer's pointer ended.
 *  Copy constructing a reader from another reader to get two pointers into the same content is done by using the copy constructor with justContent=false.
 *  A parser with lookahead is an example of why you would do that.
 *
 *
 *  todo: many of the functions were named for first use, not how they work. As such the names eventually did not match other usages.
 *  todo: C++11 and C++14 iterator and lambda helpers.
 *  todo: const versions of some methods.
 *
 * Thing stuff[7];
 * Indexer<Thing> index(stuff, sizeof(stuff));//note sizeof, not countof.
 *
 * while(newstuffIncoming()&&index.hasNext()){
 *   Thing &fill= index.next();
 *   fill.load(newestuff);
 * }
 *
 * Indexer<Thing> scanner(index); //default args are to make a reader from a writer
 * for(scanner.rewind();scanner.hasNext();){//no rewind needed if index isn't used between creation and this loop
 *   Thing &scan= scanner.next(); //creating a local reference isn't always needed ...
 *   scan.dickWithIt();
 * }
 *
 */

template<typename Content> class CircularIndexer;  //so we can cyclically access a subset of one of the following.

template<typename Content> class Indexer : public LatentSequence<Content>, public Ordinator {
  friend class CircularIndexer<Content>; //so we can cyclically access a subset of one of these.
protected:
  Content *buffer;
private:
  /** copy @param qty from start of source to end of this. This is a raw copy, no new objects are created */
  void catFrom(Indexer<Content> &source, unsigned qty){
    if(stillHas(qty) && qty <= source.used()) {
      copyObject(&source.peek(), &peek(), qty * sizeof(Content));
      source.skip(qty);
      skip(qty);
    }
  }

public:
  /** make a useless one */
  Indexer(void) : Ordinator(0),
    buffer(0){
    //#nada
  }

  /** 1st arg is const'ed even though the class doesn't guarantee that it won't hand out a pointer to an element later, user beware*/
  //#below: truncating divide, omit attempt to have partial last element.  /*-1: key value for 'same as length'*/
  Indexer(Content *wrapped, unsigned int sizeofBuffer, bool wrap = false) : Ordinator(sizeofBuffer / sizeof(Content), wrap ? -1 : 0), buffer(wrapped){
    //#nada
  }

  /* if @param content is true then the new indexer covers just the data before the old one's pointer minus the clip value,
   * some would call that a left subset of the used part.
   * else if it covers the old one's allocated range truncated by the clip value (left subset of allocated)*/
  Indexer(const Indexer &other, bool justContent = true, unsigned int clip = 0) : Ordinator((justContent ? other.pointer : other.length) - clip),
    buffer(other.buffer){
  }

  /** reworks this one to be used region of @param other.
   *   carefully implemented so that idx.snap(idx) works sensibly.*/
  void snap(const Indexer &other){
    buffer = other.buffer;
    length = other.pointer;
    pointer = 0;
  }

  /** reduce length to be that used and reset pointer.
   * useful for converting from a write buffer to a read buffer, but note that the original buffer size is lost.*/
  void freeze(){
    snap(*this);
  }

  /** substring starting from present pointer, if requested length overruns end of buffer return value is truncated.
   * This does not 'new' anything, the compiler hopefully can skip the implied copy  */
  Indexer<Content> subset(unsigned fieldLength,bool removing = true){
    int length = lesser(fieldLength,freespace());
    Indexer<Content> sub(&peek(),length);
    if(removing) {
      skip(length);
    }
    return sub;
  }

  /** forget present buffer and record start and length of some other one. */
  Indexer<Content> &wrap(Content *wrapped, unsigned int sizeofBuffer){
    pointer = 0;
    length = sizeofBuffer / sizeof(Content);
    buffer = wrapped;
    return *this;
  }

  /** reworks this one to be just like @param other. snap() usually is what you want rather than this.
   * this is useful for threadsafeness, especially over keeping a pointer to the other.*/
  void clone(const Indexer &other){
    buffer = other.buffer;
    length = other.length;
    pointer = other.pointer;
  }

  /** if @param other has a non-zero pointer (such as one that is being written to) then this is set to the front end of the other,
   * else this wraps the whole of the other */
  void grab(Indexer &other){
    buffer = other.buffer;
    if(other.pointer > 0) { //want front end.
      length = other.pointer;
      pointer = 0;
    } else { //was already rewound and truncated
      length = other.length;
      pointer = other.pointer;
    }
  }

  /** actually put 0's into the buffer starting at @param ender */
  void truncate(unsigned ender){
    while(ender < pointer) {
      buffer[ender++] = 0;
    }
  }

  /** reworks this one to be a byte accessor of the filled portion of another one,
   * which usually only makes sense if this one's Content type is 8 bit.
   * hmm, can do 32 <->16 bit conversions if both types are little endian.*/
  template<typename Other> void punt(const Indexer<Other> &other){
    buffer = reinterpret_cast<Content *>(other.buffer);
    length = other.pointer * sizeof(Other) / sizeof(Content); //always nice if Content is byte
    pointer = 0;
  }

  /** append a null terminated series, but do NOT include the null itself.
   */
  void cat(const Content *prefilled){
    if(prefilled) {
      while(!(*prefilled == 0)) {
        if(pointer < length) {
          buffer[pointer++] = *prefilled++; //expect this to copy the object
        } else {
          break;
        }
      }
    }
  } /* cat */

  void cat(Indexer<Content> &source){
    int used = source.used();
    int free = freespace();

    if(used <= free) {
      catFrom(source, used);
    } else {
      catFrom(source, free);
    }
  }

  /** number of bytes in object, ignores pointer */
  int allocated() const {
    return length;
  }

  /** @return index of next which is typically the same as the number of times 'next()' has been called*/
  unsigned int used(void) const {
    return ordinal();
  }

  //publish parts of ordinator:
  bool hasNext(void){
    return Ordinator::hasNext();
  }

  bool hasPrevious(void) const {
    return Ordinator::hasPrevious();
  }

  /** on overrun of buffer returns last valid entry*/
  Content &next(void){
    return buffer[pointer < length ? pointer++ : length - 1];
  }

  Content next(Content onEmpty){
    return pointer < length ? buffer[pointer++] : onEmpty;
  }

  /** @returns indexth element, <b>modulo length<b> for invalid indexes. */
  Content &operator [](unsigned int index) const {
    return buffer[index < length ? index : index % length];
  }

  //syntactic sugar:
  operator bool() const {
    return hasNext();
  }

  Content &operator *(void){
    return buffer[pointer < length ? pointer : length - 1];
  }

  /** you should avoid using this value for anything except diagnostics, it allows you to bypass the bounds checking which is the reason for existence of this class. */
  Content *internalBuffer(void) const {
    return buffer;
  }

  /** @return current object ('s reference), rigged for sensible behavior when buffer is used circularly*/
  Content &peek(void){
    return buffer[pointer < length ? pointer : 0];
  }

  /**@return reference to item most likely delivered by last call to next()*/
  Content &previous(void){
    return buffer[pointer >= length ? length - 1 : (pointer ? pointer - 1 : 0)];
  }

  operator Content(){
    return previous();
  }

  /** undo last next, undo at zero is still at 0. */
  void unget(void){
    Ordinator::rewind(1);
  }

  /** needed to resolve between Sequence::skip and Ordinator::skip*/
  virtual void skip(unsigned int amount){
    Ordinator::skip(amount);
  }

  /** remove at most the given number of items preceding next.
   *  first use is processing escaped chars in a string
   */
  Indexer &remove(unsigned int amount){
    if(amount > pointer) {
      amount = pointer;
    }
    copyObject(&buffer[pointer - amount], &buffer[pointer], (length - pointer) * sizeof(Content));
    Ordinator::remove(amount);
    return *this;
  }

  /** if next() has been called at least once then replace the item that next() return with the given one.
   * this is only safe to call with Content that tolerates 'operator='
   *  if the pointer is at the end then the last item gets clobbered.
   */
  void replacePrevious(Content &item){
    if(hasPrevious()) {
      buffer[pointer - 1] = item;
    }
  }

  /** append @param other 's 0 through pointer-1 to this, but will append all or none and leaves @param other unmodified */
  Indexer appendUsed(const Indexer<Content> &other){
    int used = other.used();

    if(stillHas(used)) {
      Indexer<Content> cat(other); //make a non-const indexer around same data
      catFrom(cat, used);
    }
    return *this;
  }

  /** append @param other 's pointer through length-1 to this, but will append all or none.
   * Suitable for picking up the end of a partially copied buffer */
  Indexer appendRemaining(Indexer<Content> &other){
    int qty = other.emptySpace();
    if(stillHas(qty)) {
      catFrom(other, qty);
    }
    return *this;
  }

  /** append all or none of the allocation of @param other to this, leaving @param other untouched. */
  Indexer appendAll(const Indexer<Content> &other){
    if(stillHas(other.length)) {
      Indexer<Content> cat(other, false, 0);
      catFrom(cat, other.length);
    }
    return *this;
  }

  /** set unused content to 0. pointer is unmodified
   * only makes sense if Content=0 assignment makes sense*/
  void clearUnused()const{
    for(int i=pointer;i<allocated();i++){
      buffer[i]=0;
    }
  }

}; // class Indexer

//the following probably doesn't work, or only works for simple types:
#define IndexerWrap(thingy, wrapper) Indexer<typeof(thingy)> wrapper(&thingy, sizeof(thingy))

//raw (bytewise) access to object
#define IndexBytesOf(indexer, thingy) Indexer<u8> indexer(reinterpret_cast<u8 *>(&thingy), sizeof(thingy))

#define BytesOf(thingy) IndexBytesOf(, thingy)

// iterate. todo: replace with C++11/14 stuff.
#define ForIndexed(classname, indexer) for(Indexer<classname> list(indexer); list.hasNext(); )


#endif // bufferH
