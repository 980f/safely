#ifndef ORDINATOR_H
#define ORDINATOR_H

/** utility class for generator a sequence of integers up to a limit.*/
class Ordinator {
protected:
  //could use size_t's, but that would be painful and we won't ever get close to 2**32 with this codebase.
  unsigned pointer;
  unsigned length;
public:
  Ordinator(unsigned length, int pointer = 0 ):pointer(pointer),length(length){
    if(pointer<0){//convenience to set pointer to length
      this->pointer=length;
    }
    if(this->length < this->pointer) {//was a bug fix, might be nice to chase down usages and eliminate at source
      this->length = this->pointer; //normalize error due to bad clip value in some use cases of Indexer.
    }
  }

  /**  @return whether index is in the present data*/
  bool contains(unsigned index) const {
    return index < pointer;
  }

  /**  @return whether index might be valid someday */
  bool canContain(unsigned index) const {
    return index < length;
  }

  /** @returns whether pointer is not past the last */
  bool hasNext(void) {
    return length > 0 && pointer < length;
  }

  /** @returns whether pointer is not at first element */
  bool hasPrevious(void) const {
    return pointer > 0;
  }

  /** available for write, or if just snapped available for parsing*/
  int freespace(void) const {
    return length - pointer;
  }

  /** available for block write*/
  bool stillHas(int howmany) const {
    return freespace() >= howmany;
  }

  //NB: a cast to integer was annoying to derived types, from which this class is an extracted base.
  int ordinal(void) const {
    return pointer;
  }

  /**AFTER a next this is the index of what that next() returned.*/
  int present(void )const {
    return pointer-1;
  }

  void rewind(void){
    pointer = 0;
  }
  /**
    * subtract @param backup from pointer, stopping at 0 if excessive.
    * @see rewind(void) for complete rewind rather than using a hopefully big enough value.
    * @return this
    */
  void rewind(unsigned int backup){
    if(backup <= pointer) {
      pointer -= backup;
    } else {
      pointer = 0;
    }
  }

  void skip(unsigned int amount){
    pointer += amount;
    if(pointer > length) {
      pointer = length; //our normal one-past-end state, to make sure multiple skips don't wrap.
    }
  }

  /** ensure subsequent hasNext() reports 'no' */
  void dump(){
    pointer = length; //our normal one-past-end state, to make sure multiple skips don't wrap.
  }

  /** remove at most the given number of items preceding next.
    *  first use is processing escaped chars in a string in buffer.h, probably not much use elsewhere.
    */
  void remove(unsigned int amount){
    if(amount > pointer) {
      amount = pointer;
    }
    pointer -= amount;
    length -= amount;//todo:2 this is probably a bug for SOME uses, the buffer doesn't change size just cause we discard an entry.
  }

  void grab(const Ordinator &other){
    if(other.pointer>0){//want front end.
      length = other.pointer;
      pointer = 0;
    } else { //was already rewound and truncated
      length = other.length;
      pointer = other.pointer;
    }
  }
};

#endif // ORDINATOR_H
