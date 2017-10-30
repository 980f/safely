#ifndef ORDINATOR_H
#define ORDINATOR_H

#include "index.h"
/** utility class for generator a sequence of integers up to a limit.
 * This is built for the needs of the Indexer<> class, which explains the comments and some curious design choices.
 */
class Ordinator {
protected:
  //could use size_t's, but that would be painful and we won't ever get close to 2**32 with this codebase.
  unsigned pointer;
  unsigned length;
public:

  /** create a range of sorts */
  Ordinator(unsigned length, unsigned pointer = 0 ) : pointer(pointer),length(length){
    if(pointer>length) {//bad value: will normalize to 'consumed'. The cast to unsigned makes negative numbers HUGE.
      this->pointer = length;
    }
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
  /** create a subset of @param other. In all cases pointer will be 0, you can't back this baby up past the beginning of the other.
   * if @param clip<0 then the 'used' subset, shortened by the ~clip value (~0 gets all, ~1 loses the most recent one)
   * if @param clip>=0 then the 'unused' subset, skipping clip  items (presuming caller moves pointer to the coordinate place)
   */
  Ordinator(const Ordinator &other,int clip) :
    pointer(0),
    length(0){
    if(clip<0) {  //section past, usually 0..pointer-1,
      if(~clip<=other.pointer){
      length = other.pointer - (~clip);
      }//else stays at 0
    } else { //end section pointer .. length -1,
      length = other.freespace();
      if(length>=clip){
        length-=clip;
      } else {
        length=0;
      }
    }
    //else constructor init list has taken care of it.
  }
#pragma GCC diagnostic pop

  Ordinator(const Ordinator &other)=default;//simple copy

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
  unsigned freespace(void) const {
    return length - pointer;
  }

  /** available for block write*/
  bool stillHas(unsigned howmany) const {
    return freespace() >= howmany;
  }

  //NB: a cast to integer was annoying to derived types, from which this class is an extracted base.
  unsigned ordinal(void) const {
    return pointer;
  }

  /**AFTER a next this is the index of what that next() returned.
   * @returns ordinal of item that next() returned, ~0 if next() never called.
*/
  unsigned present(void ) const {
    return pointer - 1;
  }

  /**
   * subtract @param backup from pointer, stopping at 0 if excessive.
   * the default will result in a rewind to the beginning.
   * @return this
   */
  void rewind(unsigned backup=BadLength){
    if(backup <= pointer) {
      pointer -= backup;
    } else {
      pointer = 0;
    }
  }

  void skip(unsigned amount){
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
  void remove(unsigned amount){
    if(amount > pointer) {
      amount = pointer;
    }
    pointer -= amount;
    length -= amount;//todo:2 this is probably a bug for SOME uses, the buffer doesn't change size just cause we discard an entry.
  }

  /** if other isn't at its start then we get the part past, else we get a copy */
  void grab(const Ordinator &other){
    if(other.pointer>0) {//want front end.
      length = other.pointer;
      pointer = 0;
    } else { //was already rewound and truncated
      length = other.length;
      pointer = other.pointer;
    }
  }

}; // class Ordinator

#endif // ORDINATOR_H
