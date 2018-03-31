#pragma once //(C) 2017 Andrew Heilveil
/** a string that it is hard to go past the end of.
 * allocate a buffer and a pointer manager for it.
 */

#include "eztypes.h"
#include "charformatter.h"

template<int Size> class SafeStr : public CharFormatter {
  char content[Size];
public:
  SafeStr(void) : CharFormatter(content, Size){
    //#nada
  }

  /** @returns copy constructor usable clone of this*/
  CharScanner contents(){
    return CharScanner(*this);
  }

  void restore(){
    wrap(content, Size);
  }

  /**copy as much of the @param other content as we have room for */
  template<int Othersize> void operator =(SafeStr<Othersize> &other){
    rewind();
    cat(other.asciiz());
  }

  bool matches(const char *s){
    if(ordinal() == Size) {
      return CharScanner::matches(s);
    } else {
      return *this == s;
    }
  }

  const char *c_str(){
    return asciiz();
  }

}; // class SafeStr

