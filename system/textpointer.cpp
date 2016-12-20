#include "textpointer.h"
#include "string.h"  //strdup
#include "stdlib.h"  //free


Text::Text() : Cstr(nullptr){
  //all is well
}

Text::Text(unsigned size):Cstr ( static_cast<TextKey>( calloc(size+1,1)))
{

}

Text::Text(const char *ptr,bool takeit) : Cstr(nonTrivial(ptr) ? (takeit? ptr : strdup(ptr)) : nullptr){

}

Text::~Text(){
  clear(); //using clear instead of just free as a guard against using this after it is free'd.
}

TextKey Text::operator =(TextKey other){
  copy(other);
  return other;
}

void Text::take(TextKey other){
  clear();
  ptr=other;
}

void Text::copy(TextKey other){
  if(ptr != other) { //# if not passed self as a pointer to this' storage.
    clear();
    if(nonTrivial(other)) {
      ptr = strdup(other);
    }
  }
}

void Text::clear(){
  free(const_cast<char *>(ptr));
  ptr = nullptr;
}
