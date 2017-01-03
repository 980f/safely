#include "charscanner.h"
#include "cstr.h"

//this one is sharable, with care! You should never be calling wrap or clone on a reference.
CharScanner CharScanner::Null;

//note: we check for null termination here, so if the length is too long, we bail
int ourStrncmp(const char *one, const char *two, unsigned length){
  for(unsigned i = 0; i<length; ++i) {
    if(one[i] == '\0' || two[i] == '\0') {
      return ~0; //the function encountered a null and assumes it's a terminator.
    }
    if(one[i] > two[i]) {
      return 1;
    }
    if(one[i] < two[i]) {
      return -1;
    }
  }
  return 0;
} // ourStrncmp

bool ByteScanner ::putBytes(unsigned value, int numBytes){
  if(stillHas(numBytes)) {
    const u8 *p = reinterpret_cast<const u8 *>(&value);
    while(numBytes-- > 0) {
      next() = *p++;
    }
    return true;
  } else {
    return false;
  }
} /* putBytes */

u32 ByteScanner ::getU(int numBytes, u32 def){
  //using a pointer to a local precludes compiler optimizing for register use.
  if(stillHas(numBytes)) {
    u32 acc = 0;
    copyObject(&peek(),&acc,numBytes);
    skip(numBytes);
    return acc;
  } else {
    return def;
  }
} // ByteScanner::getU

void ByteScanner::grab(CharScanner &other){
  //#invalid cast:    grab(reinterpret_cast<ByteScanner>(other));
  buffer = reinterpret_cast<u8*>(other.buffer);
  if(other.ordinal()>0) {//want front end.
    length = other.pointer;
    pointer = 0;
  } else { //was already rewound and truncated
    length = other.length;
    pointer = other.pointer;
  }
} // ByteScanner::grab

ByteScanner::ByteScanner(void) : Indexer<u8 >(){
  //#nada
}

ByteScanner::ByteScanner(u8  *content, unsigned size ) : Indexer<u8 >(content, size){
  //#nada
}

ByteScanner::ByteScanner(const ByteScanner&other, int clip ) : Indexer<u8 >(other, clip){
  //#nada
}

ByteScanner::ByteScanner(const CharScanner&other ) : //choices herein are for first use which is in type casting a ready-to-send string.
  Indexer<u8 >( reinterpret_cast<u8*>(other.internalBuffer()), other.used()){
  //#nada
}

u16 ByteScanner ::getU16(u16 def){
  return getU(2, def);
}

u32 ByteScanner ::getU24(u32 def){
  return getU(3, def);
}

u32 ByteScanner ::getU32(u32 def){
  return getU(4, def);
}

bool ByteScanner ::putU16(unsigned value){
  return putBytes(value, 2);
}

bool ByteScanner ::putU24(unsigned value){
  return putBytes(value, 3);
}

bool ByteScanner ::putU32(unsigned value){
  return putBytes(value, 4);
}

ByteScanner ByteScanner::subset(unsigned fieldLength, bool removing){
  Indexer<u8> punter(Indexer<u8>::subset(fieldLength,removing));
  return ByteScanner(punter.internalBuffer(),punter.allocated());
}

void ByteScanner::chuckSpaces(){
  while(hasNext() && this->next() == ' ') {
    //remain calm
  }
  previous();  //we want to be pointing at the space just before the next item, so a call to next() will return the first relevant character
}

///////////////////
CharScanner::CharScanner(void) : Indexer<char >(){
  //#nada
}

CharScanner CharScanner::infer(TextKey content){
  Cstr wrap(content);
  return CharScanner(wrap.violated(),wrap.length());
}

CharScanner::CharScanner(char  *content, unsigned size ) : Indexer<char >(content, size){
  //#nada
}

CharScanner::~CharScanner(){

}

CharScanner::CharScanner(const CharScanner&other, int clip ) : Indexer<char >(other, clip){
  //#nada
}

/** grabs used part */
CharScanner::CharScanner(const ByteScanner&other, int clip) : //
  Indexer<char >(reinterpret_cast<char *>(other.internalBuffer()),clip ? other.used() : other.allocated()){
  //#nada
  pointer = 0;
}

CharScanner::CharScanner(const CharScanner &other) : CharScanner(other,0){

}

/** ensure content is null terminated at present pointer.
 * maydo: return null if we can't put a null at the end
 * maydo: add argument for 'urgent' or not, and if not urgent see if there is a null before the end, not just at the end
 */
TextKey CharScanner::asciiz(void){
  if(length == 0) { //then we don't have a place for a terminating null
    return ""; //so point to a universal empty string.
  }
  if(!hasPrevious() || previous() != 0) { //if not already terminated nicely
    if(hasNext()) { //add a terminator if we can
      next() = 0;
    } else { //else destroy the last char, termination if far more important then completeness of the string.
      //if we were prone to throw exceptions we would do that here
      previous() = 0;
    }
  }
  return U8Z(internalBuffer());
} /* asciiz */

bool CharScanner::isTerminal(){
  return !hasNext() || buffer[pointer]==0;
}

bool CharScanner::operator == (const CharScanner &rhs) const {
  CharScanner me(*this);
  CharScanner other(rhs);
  while(me.hasNext()) {
    if(other.hasNext()) {
      if(me.next() != other.next()) {
        return false; //char mistmatched
      }
    } else {
      return !me.next(); //this startswith literal but  is longer
    }
  }
  return !other.hasNext() || !other.next(); //this is shorter
} // ==

/** for use with trusted rhs strings */
#include "string.h"
bool CharScanner::operator == (const char *literal) const {
  if(!literal) {
    return used()==0; //null pointer matches empty string
  }
  /** todo: get rid of the following strncmp */
  if(0==ourStrncmp(internalBuffer(),literal,used())) {//then we MIGHT have a match
    //strncmp stops at first null in either string, or after 'used' items
    if(buffer[pointer - 1]) {//no null terminator
      return literal[pointer]==0;
    } else {
      return literal[pointer - 1]==0;
    }
  } else {
    return false;
  }
} // ==

bool CharScanner::matches(const char *s){
  if(!s) {
    return false; //never match a null pointer to anything herein, even an empty one.
  }
  CharScanner me(*this);

  while(me.hasNext() && *s) {
    if(*s++!=me.next()) {
      return false;
    }
  }
  return !me.hasNext()||me.next()==0;//occasionally we get a null term on our counted string.
} // CharScanner::matches

void CharScanner::operator =(CharScanner &other){
  rewind();
  cat(other.asciiz());
}

void CharScanner::trimNulls(void){
  while(hasPrevious() && previous() == 0) {
    unget();
  }
}

#include "cheaptricks.h"
#include "cstr.h"
bool CharScanner::isBlank(){
  if(length==0) {
    return true;
  }
  CharScanner content(*this);
  while(content.hasNext()) {
    char ch(content.next());
    if(!ch) {
      return true;//c string rule
    }
    if(!isPresent("\t\n ",ch)) {
      return false;
    }
  }
  return true;
} // CharScanner::isBlank

///////////////////
ByteLooker::ByteLooker(const u8  *content, unsigned size ) : Indexer<const u8 >(content, size){
  //#nada
}

u32 ByteLooker ::getU(int numBytes, u32 def){
  //using a pointer to a local precludes compiler optimizing for register use.
  if(stillHas(numBytes)) {
    u32 acc = 0;
    copyObject(&peek(),&acc,numBytes);
    skip(numBytes);
    return acc;
  } else {
    return def;
  }
} /* getU */
