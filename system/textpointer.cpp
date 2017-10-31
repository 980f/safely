#include "safely.h"
#include "textpointer.h"
#include "string.h"  //strdup
#include "stdlib.h"  //free
#include <utility>

#include "logger.h"
static Logger tbg("TextPointer",false);

Text::Text() : Cstr(){
  //all is well
  tbg("empty construct %p:%p",this,ptr);
}

Text::Text(TextKey other){
  tbg("const cstr %p:%p  <-%p",this,ptr,&other);
  copy(other);
}

Text::Text(unsigned size) : Cstr( static_cast<TextKey>( calloc(Zguard(size),1))){
  //we have allocated a buffer and filled it with 0
  tbg("const by size %p:%p  [%u+1]",this,ptr,size);
}

/** this guy is criticial to this class being performant. If we flub it there will be scads of malloc's and free's. */
Text::Text(Text &&other) : Cstr(other){
  tbg("construct by && %p:%p",this,ptr);
  other.release();//take ownership, clearing the other one's pointer keeps it from freeing ours.
}

/** this guy is criticial to this class being performant. If we flub it there will be scads of malloc's and free's. */
Text::Text(Text &other) : Cstr(other){
  tbg("construct by && %p:%p",this,ptr);
  other.release();//take ownership, clearing the other one's pointer keeps it from freeing ours.
}


Text::Text(TextKey other, const Span &span):Cstr(TextKey(nullptr)){
  if(nonTrivial(other)&&span.ordered()) {
    unsigned length = span.span();
    char *ptr = reinterpret_cast<char *>(malloc(Zguard(length)));
    if(ptr) {
      ptr[length] = 0;//safety null
      memcpy(ptr,&other[span.lowest],length);
      this->ptr=ptr;
      tbg("construct by span %p:%p",this,ptr);
    }
  } else {
      tbg("construct by span is null");
  }

}

Text::Text(const char *ptr,bool takeit) :
  Cstr( nonTrivial(ptr) ? (takeit ? ptr : strdup(ptr)) : nullptr){
  //we now own what was passed, or the duplicate we created.
  tbg("construct by takeit:%b  %p:%p",takeit,this,ptr);
}

Text::~Text(){
  tbg("destructor of %p:%p",this,ptr);
  clear(); //using clear instead of just free as a guard against using this after it is free'd.
}

Text::operator TextKey() const {
  return Cstr::c_str();
}

void Text::take(Text &&other){
  if(ptr != other) { //# if not passed self as a pointer to this' storage.
    tbg("taking by ref %p:%p  <-%p:%p",this,ptr,&other,other.ptr);
    clear();
    ptr = other.ptr;
    other.release();
  }
}

void Text::setto(const TextKey &other){
  if(ptr != other) { //# if not passed self as a pointer to this' storage.
    tbg("taking by cstr %p:%p  <-%p",this,ptr,other);
    clear();
    ptr = other;
  }
  //else self and we already own ourself.
}

void Text::copy(TextKey other){
  tbg("copying by cstr %p:%p  <-%p",this,ptr,other);

  if(ptr && ptr != other) { //# if not passed self as a pointer to this' storage.
    clear();
  }
  if(nonTrivial(other)) {//definitely some data
    ptr = strdup(other);//don't delete!
  }
  tbg("copied by cstr %p:%p  <-%p",this,ptr,other);
} // Text::copy

TextKey Text::operator =(const TextKey &other){
  tbg("oper eq by cstr %p:%p  <-%p",this,ptr,other);
  copy(other);
  return other;
}

void Text::take(TextKey &other){
  take(const_cast<const TextKey&>(other));
  other=nullptr;
  tbg("took cstr %p:%p  <-%p",this,ptr,other);
}

void Text::clear() noexcept {
  tbg("about to clear %p:%p",this,ptr);
  free(violate(ptr));
  release();
}
/////////////////

Text::Chunker::Chunker(const char *start):base(start){
  //#nada
}

Text Text::Chunker::operator()(unsigned leap){
  Text piece (base,*this);
  leapfrog(leap);
  return piece;
}
