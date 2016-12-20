#ifndef CHARSCANNER_H
#define CHARSCANNER_H

#include "buffer.h"

class ByteScanner;

struct CharScanner : public Indexer <char> {
  friend class ByteScanner;//to allow psuedo casting
public:
  static CharScanner Null;
  CharScanner(void);
  CharScanner(char  * content, unsigned size);
  /** make a new pointer into an existing buffer, the portion of thereof selected by
   * @param portion is -1 for 0 to pointer, +1 for pointer to allocated, 0 for 0 to allocated.
   * E.G. parsing a buffer that was just filled
 */
  CharScanner(const CharScanner &other, int portion);
  CharScanner(const ByteScanner &other, int portion);

  /**intended to undo asciiz() before cat'ing more content*/
  void trimNulls(void);//.
  /** ensure content is null terminated, then return pointer to it.
    * maydo: return null if we can't put a null at the end
    * maydo: add argument for 'urgent' or not, and if not urgent see if there is a null before the end, not just at the end
    */
  const char *asciiz(void);
  /** pointer past end OR pointing to a null */
  bool isTerminal();
  bool operator == (const CharScanner &rhs)const;
  /** for use with trusted strings */
  bool operator == (const char *literal)const;

  /** compare equal to s, ignoring s being longer.
  created to fix truncated lookup in Storable::child()*/
  virtual bool matches(const char*s);

  /** copy content, cf: clone() which just points into the other one's buffer!*/
  void operator =(CharScanner &other);
  bool isBlank();

  /** @return a null terminated subset of this, modifying this to omit the returned part and the separator */
  CharScanner split(char separator){
    CharScanner lookahead(&peek(), freespace()); //remainder of 'this'.
    while(lookahead.hasNext()) {
      if(lookahead.next() == separator) {
        lookahead.previous()=0;//drop separator leading part.
        break;
      }
    }
    skip(lookahead.used()); //adjust this to point past separator
    return CharScanner(lookahead);//return trailing piece
  }

};

/** concrete class wrapped around a template, for added functions*/
struct ByteScanner : public Indexer <u8> {
  ByteScanner (void);
  ByteScanner (u8  * content, unsigned size);
  ByteScanner (const ByteScanner &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteScanner (const CharScanner &other);

  /**for embedded binary data */
  u16 getU16(u16 def = 0);
  u32 getU24(u32 def = 0);
  u32 getU32(u32 def = 0);

  /**for embedding binary data */
  bool putU16(unsigned value);
  bool putU24(unsigned value);
  bool putU32(unsigned value);

  //used internally, but also useful externally
  bool putBytes(unsigned value, int numBytes);
  u32 getU(int numBytes, u32 def=0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void grab(CharScanner&other);

  void grab(ByteScanner&other){
    Indexer<u8>::grab(other);
  }

  void grab(Indexer<u8>&other){
    Indexer<u8>::grab(other);
  }
  /** access to base class version is a syntactic cf */
  ByteScanner subset(unsigned fieldLength, bool removing=true);
  /** Positions the pointer just before the next character that is not a space (' ') */
  void chuckSpaces();

};

struct ByteLooker : public Indexer <const u8> {
  ~ByteLooker ();
  ByteLooker (const u8 * content, unsigned size);
  ByteLooker(const ByteLooker &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteLooker (const CharScanner &other);

  /**for embedded binary data */
  u16 getU16(u16 def = 0);
  u32 getU24(u32 def = 0);
  u32 getU32(u32 def = 0);

  u32 getU(int numBytes, u32 def=0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void grab(CharScanner&other){
    //#invalid cast:    grab(reinterpret_cast<ByteScanner>(other));
    buffer = reinterpret_cast<const u8*>(other.internalBuffer());
    Ordinator::grab(other);/*
    if(other.ordinal()>0){//want front end.
      length = other.ordinal();
      pointer = 0;
    } else { //was already rewound and truncated
      length = other.length;
      pointer = other.ordinal();
    }*/
  }

};


#endif // CHARSCANNER_H
