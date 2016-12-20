#include "textpointer.h"
#include "string.h"  //strdup
#include "stdlib.h"  //free


Text::Text() : Cstr(nullptr){
  //all is well
}

Text::Text(const char *ptr) : Cstr(nonTrivial(ptr) ? strdup(ptr) : nullptr){

}

Text::~Text(){
  clear(); //using clear instead of just free as a guard against using this after it is free'd.
}

TextKey Text::operator =(TextKey other){
  if(ptr != other) { //# if not passed self as a pointer to this' storage.
    clear();
    if(nonTrivial(other)) {
      ptr = strdup(other);
    }
  }
  return other;
}

void Text::clear(){
  free(const_cast<char *>(ptr));
  ptr = nullptr;
}
