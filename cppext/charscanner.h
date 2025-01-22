#pragma once

#include "buffer.h"
#include "textkey.h"

/** signed char and unsigned char variants need to be translatable to the other */
class ByteScanner;

struct CharScanner : Indexer<char> {
  friend class ByteScanner;//to allow psuedo casting
public:
  static CharScanner Null;
  CharScanner();
  /** risks calling strlen on content. Caller must ensure @param content is a null terminated string. */
  static CharScanner infer(TextKey content);
  CharScanner(char  * content, unsigned size);
  virtual ~CharScanner() = default;
  /** explicit copy constructor helps compiler out, better than defaulting portion to 0*/
  CharScanner(const CharScanner &other);
  CharScanner(const Indexer&other);
  CharScanner(const Indexer<uint8_t>&other);

  /** make a new pointer into an existing buffer, the portion of thereof selected by
   * @param portion is -1 (~0) for 0 to pointer, +1 for pointer to allocated, 0 for 0 to allocated.
   * E.G. parsing a buffer that was just filled
   */
  CharScanner(const CharScanner &other, int portion);
  CharScanner(const ByteScanner &other, int portion);

  /**intended to undo asciiz() before cat'ing more content*/
  void trimNulls();//.
  /** ensure content is null terminated, then return pointer to it.
   * maydo: return null if we can't put a null at the end
   * maydo: add argument for 'urgent' or not, and if not urgent see if there is a null before the end, not just at the end
   */
  TextKey asciiz();
  /** pointer past end OR pointing to a null */
  bool isTerminal();
  bool operator == (const CharScanner &rhs) const;
  /** for use with trusted strings */
  bool operator == (const char *literal) const;

  /** compare equal to s, ignoring s being longer.
   *  created to fix truncated lookup in Storable::child()*/
  virtual bool matches(const char*s);

  /** copy content, cf: clone() which just points into the other one's buffer!*/
  void operator =(CharScanner &&other) noexcept;
  bool isBlank() const;

  /** @return a null terminated subset of this, modifying this to omit the returned part and the separator.
   *  If the separator is not found then the remainder of the buffer is returned.*/
  CharScanner cut(char separator); //may have once been name 'split'

  /** added this modality instead of dusting off yet another variant of this class (Bigendianer) */
  bool bigendian = false;
  bool putBytes(unsigned value, unsigned numBytes);
  uint32_t getU(unsigned numBytes, uint32_t def);
  /**for embedded binary data */
  uint16_t getU16(uint16_t def = 0);
  uint32_t getU24(uint32_t def = 0);
  uint32_t getU32(uint32_t def = 0);

  /**for embedding binary data */
  bool putU16(unsigned value);
  bool putU24(unsigned value);
  bool putU32(unsigned value);
};

/** concrete class wrapped around a template, for added functions*/
class ByteScanner : public Indexer<uint8_t> {
public:
  ByteScanner();
  ByteScanner(uint8_t  * content, unsigned size);
  ByteScanner(const ByteScanner &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteScanner(const CharScanner &other);
  virtual ~ByteScanner() = default;

  /** added this modality instead of dusting off yet another variant of this class (Bigendianer) */
  bool bigendian = false;
  /**for embedded binary data */
  uint16_t getuint16_t(uint16_t def = 0);
  uint32_t getU24(uint32_t def = 0);
  uint32_t getuint32_t(uint32_t def = 0);

  /**for embedding binary data */
  bool putuint16_t(unsigned value);
  bool putU24(unsigned value);
  bool putuint32_t(unsigned value);

  //used internally, but also useful externally
  bool putBytes(unsigned value, unsigned numBytes);
  uint32_t getU(unsigned numBytes, uint32_t def = 0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void getTailOf(const CharScanner&other);

  void getTail(const ByteScanner&other){
    getTailOf(other);
  }

  void getTail(Indexer&other){
    getTailOf(other);
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

struct ByteLooker : public Indexer<const uint8_t> {
  virtual ~ByteLooker() = default;
  ByteLooker(const uint8_t * content, unsigned size);
  ByteLooker(const ByteLooker &other, int clip = 0);
  //casting constructor, pointer to existing content like a snap().
  ByteLooker(const CharScanner &other);

  /**for embedded binary data */
  uint16_t getuint16_t(uint16_t def = 0);
  uint32_t getU24(uint32_t def = 0);
  uint32_t getuint32_t(uint32_t def = 0);

  uint32_t getU(unsigned numBytes, uint32_t def = 0);

  /**had to copy from the base class, couldn't figure out how to cast one template into another.*/
  void getTail(CharScanner&other){
    //#invalid cast:    grab(reinterpret_cast<ByteScanner>(other));
    buffer = reinterpret_cast<const uint8_t*>(other.internalBuffer());
    Ordinator::grab(other);
  }

};
