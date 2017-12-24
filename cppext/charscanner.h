#ifndef CHARSCANNER_H
#define CHARSCANNER_H

#include "buffer.h"
#include "textkey.h"

/** signed char and unsigned char variants need to be translatable to the other */
class ByteScanner;

struct CharScanner : public Indexer<char> {
  friend class ByteScanner;//to allow psuedo casting
public:
  static CharScanner Null;
  CharScanner(void);
  /** risks calling strlen on content. Caller must ensure @param content is a null terminated string. */
  static CharScanner infer(TextKey content);
  CharScanner(char  * content, unsigned size);
  virtual ~CharScanner() = default;
  /** explicit copy constructor helps compiler out, better than defaulting portion to 0*/
  CharScanner(const CharScanner &other);
  CharScanner(const Indexer<char>&other);
  CharScanner(const Indexer<u8>&other);

  /** make a new pointer into an existing buffer, the portion of thereof selected by
   * @param portion is -1 (~0) for 0 to pointer, +1 for pointer to allocated, 0 for 0 to allocated.
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
  TextKey asciiz(void);
  /** pointer past end OR pointing to a null */
  bool isTerminal();
  bool operator == (const CharScanner &rhs) const;
  /** for use with trusted strings */
  bool operator == (const char *literal) const;

  /** compare equal to s, ignoring s being longer.
   *  created to fix truncated lookup in Storable::child()*/
  virtual bool matches(const char*s);

  /** copy content, cf: clone() which just points into the other one's buffer!*/
  void operator =(CharScanner &&other);
  bool isBlank();

  /** @return a null terminated subset of this, modifying this to omit the returned part and the separator.
   *  If the separator is not found then the remainder of the buffer is returned.*/
  CharScanner cut(char separator); //may have once been name 'split'

  /** added this modality instead of dusting off yet another variant of this class (Bigendianer) */
  bool bigendian = false;
  bool putBytes(unsigned value, unsigned numBytes);
  u32 getU(unsigned numBytes, u32 def);
  /**for embedded binary data */
  u16 getU16(u16 def = 0);
  u32 getU24(u32 def = 0);
  u32 getU32(u32 def = 0);

  /**for embedding binary data */
  bool putU16(unsigned value);
  bool putU24(unsigned value);
  bool putU32(unsigned value);
};

/** concrete class wrapped around a template, for added functions*/
class ByteScanner : public Indexer<u8> {
public:
  ByteScanner(void);
  ByteScanner(u8  * content, unsigned size);
  ByteScanner(const ByteScanner &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteScanner(const CharScanner &other);
  virtual ~ByteScanner() = default;

  /** added this modality instead of dusting off yet another variant of this class (Bigendianer) */
  bool bigendian = false;
  /**for embedded binary data */
  u16 getU16(u16 def = 0);
  u32 getU24(u32 def = 0);
  u32 getU32(u32 def = 0);

  /**for embedding binary data */
  bool putU16(unsigned value);
  bool putU24(unsigned value);
  bool putU32(unsigned value);

  //used internally, but also useful externally
  bool putBytes(unsigned value, unsigned numBytes);
  u32 getU(unsigned numBytes, u32 def = 0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void getTail(CharScanner&other);

  void getTail(const ByteScanner&other){
    Indexer<u8>::getTail(other);
  }

  void getTail(Indexer<u8>&other){
    Indexer<u8>::getTail(other);
  }

  /** access to base class version is a syntactic cf */
  ByteScanner subset(unsigned fieldLength, bool removing = true);
  //// parser assist:
  /** Positions the pointer so that next() returns the next char that is not a space. @returns hasNext().
   * If there are no spaces then the buffer is 'consumed', you may wish to mark and rewind on failure. */
  bool chuckSpaces();

  /** if the next content of this matches other then move past the value and @returns true. else this is unchanged and @returns false */
  bool nextWord(Indexer<const char> other);//yes. copy the argument

  /** if this contains other then move past the value and @returns true. else this is unchanged and @returns false */
  bool find(Indexer<const char> other);//yes. copy the argument
}; // class ByteScanner

struct ByteLooker : public Indexer<const u8> {
  virtual ~ByteLooker() = default;
  ByteLooker(const u8 * content, unsigned size);
  ByteLooker(const ByteLooker &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteLooker(const CharScanner &other);

  /**for embedded binary data */
  u16 getU16(u16 def = 0);
  u32 getU24(u32 def = 0);
  u32 getU32(u32 def = 0);

  u32 getU(unsigned numBytes, u32 def = 0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void getTail(CharScanner&other){
    //#invalid cast:    grab(reinterpret_cast<ByteScanner>(other));
    buffer = reinterpret_cast<const u8*>(other.internalBuffer());
    Ordinator::grab(other);
  }

};


#endif // CHARSCANNER_H
