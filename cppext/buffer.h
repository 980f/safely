#pragma once //"(C) Andrew L. Heilveil, 2017-2018 "

#include "eztypes.h"
#include "minimath.h"
#include "sequence.h"
#include "ordinator.h"
#include "index.h"

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
 * The term "primary buffer" is used in comments to refer to one which describes an actuall allocation of memory whereas this class often represents a view within such.
 * It is up to the user to note when that is the case and to not use a mutator which changes the range of a primary buffer.
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


//instead of random segv's make them fault at 0
#define CppExtBufferFailureGuard  if(length==0){return NullRef(Content);}

template<typename Content> class CircularIndexer;  //so we can cyclically access a subset of one of the following.

template<typename Content> class Indexer : public LatentSequence<Content>, public Ordinator {
  friend class CircularIndexer<Content>; //so we can cyclically access a subset of one of these.
protected:
  Content *buffer;
private:
  /** only call this after idiot checking all of the args.*/
  void movem(unsigned from, unsigned to, unsigned amount){
    copyObject(buffer+from, buffer+to, (amount) * sizeof(Content));
  }


  /** copy @param qty from start of source to end of this. This is a raw copy, no new objects are created */
  void catFrom(Indexer<Content> &source, unsigned qty){
    if(stillHas(qty) && qty <= source.used()) {
      copyObject(source.internalBuffer(), this->internalBuffer(), qty* sizeof(Content));
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

  /* if @param clip is negative then the new indexer covers just the data before the old one's pointer minus the ~clip value, i.e. data already visited excluding the most recent. NB that a clip of ~0 gets everything beneath the pointer (same values as getHead), ~1 ends the new Indexer one shy of the oldone's pointer (such as removing a comma).
   * a rewind of 0 gets you the equivalent of getTail
   * a rewind>0 gets you the unvisited part of the other, with the given number of already visited elements.
   * e.g. a value of 1 after reading a comma will get you a buffer starting with that comma */
  Indexer(const Indexer &other, int clip) : //default value is clone of created state of other.
    Ordinator(other, clip),
    //if 0 or ~clipsome start is same as start of other, else start offset from this one's current location
    buffer(0){
    if(clip<=0){
     buffer= other.buffer ;
    } else {
      buffer= (other.buffer + (other.pointer + clip));
    }
  }

  /** simple copy constructor */
  Indexer(const Indexer &other)=default;

  /** deallocate contents, set tracking to reflect that. This is NEVER called from within this class, only call it if you know the content was allocated for this object and not remembered elsewhere */
  void freeContent(){
    delete [] buffer;
    buffer=nullptr;
    length=0;
    pointer=0;
  }

  /** @returns whether this seems to be a useful object. Note that it might have no freespace(), but it will have content.
 It was created to detect buffers that were created around a malloc or the like return.*/
  bool isUseful() const {
    return buffer!=nullptr && length>0 && length!=BadLength && pointer <=length;
  }

  /** @returns whether this isUseful or a perfectly empty 'null' one.*/
  bool isLegal() const {
    return isUseful() || (length==0 && buffer==nullptr && pointer==0);
  }

  /** It only makes sense to use this once right after creation of the related buffer.
   * This method zeroes out trailing part of allocation and then forgets it exists.
   * The most obvious use of this is to ensure a trailing null on strings.
   * It might also be useful in putting a trailing nullptr on an array of pointers, but we don't often use this class for that duty.
   * This is an alternative to using the Zguard function to futz the array before allocating an Indexer to view it.
   * @returns 0 on success, else how many bytes overlapped the used area of the Indexer.
*/
  unsigned zguard(unsigned amount=1){
    while(amount-->0 && length>pointer){
      buffer[--length]=Content(0);
    }
    return amount;
  }

  /** reworks this one to be visited region of @param other.
   *   carefully implemented so that idx.snap(idx) works sensibly.*/
  void getHead(const Indexer &other){
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
  void getTail(const Indexer<Content> &other){
    pointer = 0;
    buffer = &other.peek();
    length = other.freespace();
  }


  /** reduce length to be that used and reset pointer.
   * useful for converting from a write buffer to a read buffer, but note that the original buffer size is lost.*/
  void freeze(){
    getHead(*this);
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

  /** @returns an Indexer that covers the half open range defined by @param start and @param end.
   * if end is past the end (e.g. ~0) then the actual end is used.
   * if start is not valid then an empty indexer is returned.
   * This does not 'new' anything, the compiler hopefully can elide the implied copy.  */
  Indexer<Content> view(unsigned start,unsigned end)const{
    if(end>length){
      end=length;
    }
    if(start<length){
      return Indexer<Content>(&buffer[start],(end-start)*sizeof (Content));
    } else {
      return Indexer<Content>();
    }
  }

  /** @returns an indexer which covers the leading part of this one. */
  Indexer<Content> getHead() const{
    return view(0,pointer);
  }

  /** @returns an indexer which covers the trailing part of this one. once upon a time called 'remainder' */
  Indexer<Content> getTail() const{
    return view(pointer,allocated());
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
     forget();
    }
  } // trimLeading

  /** nulls content from offset ender through present pointer.
   * actually put 0's into the buffer starting at @param ender. Content is modified, this buffer object itself is unchanged. */
  void truncate(unsigned ender){
    while(ender < pointer) {
      buffer[ender++] = 0;
    }
  }

  void clipRange(Index high, Index low=0){
    if(high<length){
      length=high;
    }
    skip(low);//todo: alter buffer address so that rewind is clipped
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
   * @returns whether the whole content was copied (also true if limit ended copy)
   */
  bool cat(const Content *prefilled,unsigned limit=BadLength){
    if(prefilled) {
      //do field by field copy, Content might override * and ++ operators.
      while( (pointer < length) && (*prefilled != 0) && limit-->0) {
        buffer[pointer++] = *prefilled++; //expect this to copy the object
      }
      return *prefilled==0 || limit==BadLength;//not truncated or truncated by request.
    } else {
      return true;//cat nothing == no truncation
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
  virtual bool hasNext(void) override {//const removed to allow derived classes to lookahead and cache
    return Ordinator::hasNext();
  }

  bool hasPrevious(void) const override {
    return Ordinator::hasPrevious();
  }

  /** on overrun of buffer returns last valid entry*/
  virtual Content &next(void) override{
    CppExtBufferFailureGuard
    return buffer[pointer < length ? pointer++ : length - 1];
  }

  virtual Content next(Content onEmpty){
    return pointer < length ? buffer[pointer++] : onEmpty;
  }


  /** @returns indexth element, <b>modulo length<b> for invalid indexes. */
  Content &operator [](unsigned int index) const {
    CppExtBufferFailureGuard
    return buffer[index < length ? index : index % length];
  }

  //syntactic sugar:
  operator bool()  {
    return hasNext();
  }

  Content &operator *(void){
    CppExtBufferFailureGuard
    return buffer[pointer < length ? pointer : length - 1];
  }

  /** you should avoid using this value for anything except diagnostics, it allows you to bypass the bounds checking which is the reason for existence of this class. */
  Content *internalBuffer(void) const {
    return buffer;
  }

  /** @return current object ('s reference), rigged for sensible behavior when buffer is used circularly*/
  Content &peek(void) const override {
    CppExtBufferFailureGuard
    return buffer[pointer < length ? pointer : 0];
  }

  /** @returns reference to item most likely delivered by last call to next()*/
  Content &previous(void) const {
    CppExtBufferFailureGuard
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
  virtual void skip(unsigned int amount=1){
    Ordinator::skip(amount);
  }

  /** move pointer to an 'absolute' location. @returns whether the given location was in the buffer, if not then pointer is set to the canonical 'none left' value. */
  bool skipto(unsigned point){
    if(point<length){
      pointer=point;
      return true;
    } else {
      pointer=length;
      return false;
    }
  }

  /** remove at most the given number of items preceding next.
   *  first use is processing escaped chars in a string.
   * This does a memory move, no copy constructor or such will be invoked.
   * The allocation is reduced, don't do this to a primary buffer.
   */
  Indexer &remove(unsigned int amount){
    if(amount > pointer) {
      amount = pointer;
    }
    movem(pointer,(pointer - amount),(length - pointer));
    Ordinator::remove(amount);
    return *this;
  }

  /** remove bytes from the start of the allocation, actually moving data.
 This method does NOT alter the allocation. This only makes sense for a receive buffer as you peel items off the front. */
  Indexer &removeHead(unsigned amount){
    if(amount>=pointer){//remove all
      pointer=0;//simple ignore what we had
    } else {
      movem(amount,0,pointer-=amount);
    }
    return *this;
  }

  /** use: if you lookedahead and instead of just skipping you want to move data in the buffer.
   @returns whether the move was done, which will only happen if the @param amount is reasonable.
 @param andShrink tells whether the buffer should be shrunk to reflect removed content. */
  bool removeNext(unsigned amount,bool andShrink=true){
    unsigned start=pointer+amount;//what should be next when we are done.
    if(canContain(start)){//don't pull from past end
      //data quantity is freespace-amount being removed since we are removing from the freespace
      unsigned quantity=freespace()-amount;
      if(canContain(quantity)){//slightly bogus, but a negative quantity will fail this test.
        movem(start,pointer,quantity);
        if(andShrink){
          length-=quantity;
        }
        return true;
      }
    }
    return false;
  }

  /** seeks in head of buffer for something that operator=='s @param item.
@deprecated untested */
  unsigned findInHead(const Content &item){
    for(unsigned peek=0;peek<pointer;++peek){
      if(item==buffer[peek]){
        return peek;
      }
    }
    return BadIndex;
  }

  /** seeks in tail of buffer for something that operator=='s @param item.
   * pointer is not moved, typically you will skip in some fashion.
@deprecated untested */
  unsigned findInTail(const Content &item){
    for(unsigned peek=pointer;peek<allocated();++peek){
      if(item==buffer[peek]){
        return peek;
      }
    }
    return BadIndex;
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
    int qty = other.freespace();
    if(stillHas(qty)) {
      catFrom(other, qty);
    }
    return *this;
  }

  /** append all or none of the allocation of @param other to this, leaving @param other untouched. */
  bool appendAll(const Indexer<Content> &other){
    if(stillHas(other.length)) {
      Indexer<Content> cat(other, 0);
      cat.dump();//this was missing for a long time.
      catFrom(cat, other.length);
      return true;
    }
    return false;
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
    list.getTail(*this);
    while(list.hasNext()) {
      (*eff)(list.next());
    }
  }

  /** */
  void forget(){
    length=0;
    pointer=0;
    buffer=nullptr;
  }

  //free contents and forget them. Only safe todo if you know this buffer is created from a malloc.
  void destroy(){
    if(length){//# testing 4debug, delete[] can handle a zero.
      delete []buffer;
    }
    //and now forget we ever saw those
    forget();
  }

  /** YOU must arrange to delete the contents of what this function returns. The @see destroy() method is handy for that.
   * this method was originally created for text strings, got tired of repeating the 'make one and ensure null' paragraph
*/
  static Indexer<Content> make(unsigned quantity, bool zterm=false){
    if(Index(quantity).isValid()){
      Content *path=new Content[quantity+(zterm?1:0)];
      if(zterm){
        path[quantity]=0;//null terminate since we didn't pre-emptively calloc.
      }
      return Indexer<char> (path,quantity);
    } else {
      return Indexer();
    }
  }

}; // class Indexer
#undef CppExtBufferFailureGuard

//the following probably doesn't work, or only works for simple types:
#define IndexerWrap(thingy, wrapper) Indexer<typeof(thingy)> wrapper(&thingy, sizeof(thingy))

//you must include unistd.h before using this:
#define BuildIndexer(classname,itemname,count) itemname(reinterpret_cast<classname *>(malloc(count*sizeof(classname))),count*sizeof(classname))
#define AllocateIndexer(classname,itemname,count) Indexer<classname> BuildIndexer(classname,itemname,count)


//raw (bytewise) access to object
#define IndexBytesOf(indexer, thingy) Indexer<u8> indexer(reinterpret_cast<u8 *>(&thingy), sizeof(thingy))

//do we still need these?:
#define BytesOf(thingy) IndexBytesOf(, thingy)

#define ForIndexed(classname, indexer) for(Indexer<classname> list(indexer); list.hasNext(); )
