#ifndef BUFFER_H
#define BUFFER_H

#include "eztypes.h"
#include "minimath.h"
#include "sequence.h"
#include "ordinator.h"


/**
 *  This class is used to prevent buffer overflows.
 *  Instances are passed a pointer to a buffer and its length and provide pointer-like syntax for modifying and accessing said
 *  buffer, but won't increment the pointer outside the allocated range.
 *  This class does not CONTAIN the data, it is a pointer into data allocated elsewhere.
 *  todo: add ownership flag, to delete the buffer when the object goes away. We don't already have that as most of the time we have multiple indexers into a shared buffer.
 *  If this is the only place a malloc'd address is retained you can free that block (at the appropriate time!) by calling free(indx.internalBuffer());
 *
 *  There are two major modalities of use, writing and reading.
 *
 *  Writing starts with the pointer at zero and stuff is added until done or the pointer is at the end of allocation.
 *  One then uses the copy constructor to create a reader, with the pointer at zero and its allocation where the writer's pointer ended.
 *  Copy constructing a reader from another reader to get two pointers into the same content is done by using the copy constructor with rewind=0.
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
 * Indexer<Thing> scanner(index,~0); //~ for filled part of buffer, ~1 to omit last char ~2 for last two...
 * while(scanner.hasNext();){
 *   Thing &it= scanner.next(); //creating a local reference isn't always needed ...
 *   it.dickWithIt();
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

  /** forgets present buffer and records start and length of some other one.
   *  @returns this */
  Indexer<Content> &wrap(Content *wrapped, unsigned int sizeofBuffer){
    pointer = 0;
    length = sizeofBuffer / sizeof(Content);
    buffer = wrapped;
    return *this;
  }

  /** make a useless one */
  Indexer(void) : Ordinator(0),
    buffer(0){
    //#nada
  }

  /** #below: truncating divide, omit attempt to have partial last element. */
  Indexer(Content *wrapped, unsigned sizeofBuffer) : Ordinator(sizeofBuffer / sizeof(Content), 0U), buffer(wrapped){
    //#nada
  }

  /* if @param rewind is negative then the new indexer covers just the data before the old one's pointer minus the ~rewind value, i.e. data already visited excluding
   * the most recent. NB that a clip of ~0 gets everything beneath the pointer, ~1 ends the new Indexer one shy of the oldone's pointer (such as removing a comma).
   * a rewind of 0 gets you the equivalent of rewind(all) then clone() i.e. it ignores the other's pointer and gives you the construction time view of the other.
   * a rewind>0 gets you the unvisited part of the other, with the given number of already visited elements.
   * e.g. a value of 1 after reading a comma will get you a buffer starting with that comma */
  Indexer(const Indexer &other, int rewind = 0) : //default value is clone of created state of other.
    Ordinator(other, rewind),

    buffer(rewind<=0 ? other.buffer : other.buffer + (other.pointer - rewind)){
  }

  /** reworks this one to be used region of @param other.
   *   carefully implemented so that idx.snap(idx) works sensibly.*/
  void snap(const Indexer &other){
    buffer = other.buffer;
    length = other.pointer;
    pointer = 0;
  }

  /** reworks this one to be just like @param other. snap() usually is what you want rather than this.
   * this is useful for threadsafeness, especially over keeping a pointer to the other.*/
  void clone(const Indexer &other){
    buffer = other.buffer;
    length = other.length;
    pointer = other.pointer;
  }

  /** tail end of other, without 'removing' it from other. Very suitable for a lookahead parser */
  void grab(const Indexer<Content> &other){
    pointer = 0;
    buffer = &other.peek();
    length = other.freespace();
  }

  /** @returns an indexer that covers the freespace of this one. this one is not modified */
  Indexer<Content> remainder() const {
    Indexer<Content> rval;
    rval.grab(*this);
    return rval;
  }

  /** reduce length to be that used and reset pointer.
   * useful for converting from a write buffer to a read buffer, but note that the original buffer size is lost.*/
  void freeze(){
    snap(*this);
  }

  /** @returns an Indexer that covers just the @param fieldLength next members of this, @param removing is whether to remove them from this one's scan.
   * substring starting from present pointer, if requested length overruns end of buffer return value is truncated.
   * This does not 'new' anything, the compiler hopefully can elide the implied copy.  */
  Indexer<Content> subset(unsigned fieldLength,bool removing = true){
    int length = lesser(fieldLength,freespace());
    Indexer<Content> sub(&peek(),length*sizeof (Content));
    if(removing) {
      skip(length);
    }
    return sub;
  }

  /** reworks this to move start of buffer to be @param howmany past present start.
   * does sensible things if you trim past the current pointer, wipes the whole thing if you trim all of it.
   */
  void trimLeading(unsigned howmany){
    if(howmany<pointer) {//normal case
      buffer += howmany;//move start
      pointer -= howmany;
      length -= howmany;
    } else if(howmany<length) {//remove all past and maybe a few unseen
      buffer += howmany;//move start
      pointer = 0;//make it valid
      length -= howmany;
    } else {//kill the whole thing
      buffer = nullptr;
      pointer = 0;
      length = 0;
    }
  } // trimLeading

  /** actually put 0's into the buffer starting at @param ender. Content is modified, this buffer object itself is unchanged. */
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
   * This copies objects via operator=  , and requires a compare to 0 functionality
   */
  void cat(const Content *prefilled){
    if(prefilled) {
      while( (pointer < length) && (*prefilled != 0)) {
        buffer[pointer++] = *prefilled++; //expect this to copy the object
      }
    }
  } /* cat */

  void cat(Indexer<Content> &source){
    int used = source.used();
    int free = freespace();

    catFrom(source, used <= free? used:free);
  }

  /** number of entities in buffer, ignores pointer */
  unsigned allocated() const {
    return length;
  }

  /** @return index of next which is typically the same as the number of times 'next()' has been called*/
  unsigned int used(void) const {
    return ordinal();
  }

  //publish parts of ordinator, without these derived classes are deemed abstract.
  virtual bool hasNext(void) const {
    return Ordinator::hasNext();
  }

  bool hasPrevious(void) const {
    return Ordinator::hasPrevious();
  }

  /** on overrun of buffer returns last valid entry*/
  virtual Content &next(void){
    return buffer[pointer < length ? pointer++ : length - 1];
  }

  /** on overrun of buffer returns @param onEmpty.
NB this uses references in and out, you connot pass a const onEmpty */
//  Content &next(Content &onEmpty){
//    return pointer < length ? buffer[pointer++] : onEmpty;
//  }

  virtual Content next(Content onEmpty){
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
  Content &peek(void) const {
    return buffer[pointer < length ? pointer : 0];
  }

  /**@return reference to item most likely delivered by last call to next()*/
  Content &previous(void) const {
    return buffer[pointer >= length ? length - 1 : (pointer ? pointer - 1 : 0)];
  }

  operator Content() const {
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
   *  first use is processing escaped chars in a string.
   * This does a memory move, no copy constructor or such will be invoked.
   */
  Indexer &remove(unsigned int amount){
    if(amount > pointer) {
      amount = pointer;
    }
    copyObject(&buffer[pointer - amount], &buffer[pointer], (length - pointer) * sizeof(Content));
    Ordinator::remove(amount);
    return *this;
  }

  /** if next() has been called at least once then replace the item that next() returned with the given one.
   * this is only safe to call with Content that tolerates 'operator='
   *  if the pointer is at the start then nothing happens.
   */
  void replacePrevious(Content &item){
    if(hasPrevious()) {
      buffer[pointer - 1] = item;
    }
  }

  /** append @param other 's 0 through pointer-1 to this, but will append all or none and leaves @param other unmodified */
  Indexer appendUsed(const Indexer<Content> &other){
    unsigned used = other.used();

    if(stillHas(used)) {
      Indexer<Content> cat(other,~0); //make a non-const indexer around same data
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
      Indexer<Content> cat(other, 0);
      catFrom(cat, other.length);
    }
    return *this;
  }

  /** set unused content to 0. pointer is unmodified
   * only makes sense if Content=0 assignment makes sense*/
  void clearUnused() const {
    for(unsigned i = pointer; i<allocated(); i++) {
      buffer[i] = 0;
    }
  }

  typedef void *(functoid)(Content &each);
  /** apply @param eff function to all members of this buffer, ignoring pointer */
  void forEach(functoid eff) const {
    Indexer<Content> list(*this,0);
    while(list.hasNext()) {
      (*eff)(list.next());
    }
  }

  void forUsed(functoid eff) const{
    Indexer<Content> list(*this,~0);
    while(list.hasNext()) {
      (*eff)(list.next());
    }
  }

  void forRemaining(functoid eff) const {
    Indexer<Content> list;
    list.grab(*this);
    while(list.hasNext()) {
      (*eff)(list.next());
    }
  }

  //free contents and forget them. Only safe todo if you know this buffer is created from a malloc.
  void destroy(){
    if(length){//# testing for debug, delete[] can handle a zero.
      delete []buffer;
    }
    //and now forget we ever saw those
    length=0;
    pointer=0;
    buffer=nullptr;
  }

  /** YOU must arrange to delete the contents of what this function returns. The @see destroy() method is handy for that.
   * this method was originally created for text strings, got tired of repeating the 'make one and ensure null' paragraph
*/
  static Indexer<Content> make(unsigned quantity, bool zterm=false){
    if(Index(quantity).isValid()){
      Content *path=new Content[zterm?1+quantity:quantity];
      if(zterm){
        path[quantity]=0;//null terminate since we didn't pre-emptively calloc.
      }
      return Indexer<char> (path,quantity);
    } else {
      return Indexer();
    }
  }

}; // class Indexer

//the following probably doesn't work, or only works for simple types:
#define IndexerWrap(thingy, wrapper) Indexer<typeof(thingy)> wrapper(&thingy, sizeof(thingy))

//raw (bytewise) access to object
#define IndexBytesOf(indexer, thingy) Indexer<u8> indexer(reinterpret_cast<u8 *>(&thingy), sizeof(thingy))

#endif // bufferH
