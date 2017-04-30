#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "cstr.h"
#include "halfopen.h" //Span

/** This class is a minimal String class, it prevents memory leaks and trivial NPE's but does provide any manipulation facilities.
 * Most especially it does not rellocate/resize its contents. It uses malloc and free rather than new and delete so that it might yield a smaller binary on microcontroller systems.
 *
 * adds strdup'ing to @see Cstr functionality, i.e. makes a copy on construction and assignement vs Cstr which just looks at someone else's allocated memory.
 *
 *  This class unconditionally frees the data it points at when destroyed. IE it always owns what it points at, which is why it usually copies its argument on assignment and construction.
 */
class Text : public Cstr {

public:
  /** creates an 'empty' one */
  Text();

  /** makes a copy of the @param given content if @param takeit is false, else presumes the caller is happy with this maintaining the lifetime.
   * IE: if takeit is true then this class just records the pointer, if false it makes a copy of the data and forgets the given pointer.
   * This class *always* frees the data it points at.
 */
  Text(TextKey ptr,bool takeit);

  /** makes a copy of the @param given content */
  Text(TextKey other);

  /** calloc's a block of data. */
  Text(unsigned size);

  /** enforce that a const can't have its resource taken away from it. */
  Text(const Text &other)=delete ;

  /** take content from @param other, other will be empty */
  Text(Text &&other);

  /** take content from @param other, other will be empty */
  Text(Text &other);


  /** make a copy of non-null-terminated subset of some string. includes @param begin but not @param end */
  Text(TextKey other,const Span &span);

public:

  /** deletes its copy of the content copied or recorded by the constructor */
  virtual ~Text();

  /** useful for forcing a copy when constructing, the copy constructor is used for moving. */
  operator TextKey() const;

  /** deletes present content (if any) and copies @param ptr content (if any).
   * @returns the @param pointer, not a pointer to self or the copy made.
   */
  TextKey operator =(const TextKey &other);

  /** take ownership of a buffer, i.e. deleting this Text object will free @param other. Other will be a null poiner after this call, you should not reference it again. */
  void take(TextKey &other);

  /** take ownership of a buffer, i.e. deleting this Text object will free @param other */
  void take(const TextKey &other);

  /** take ownership of a buffer, i.e. deleting this Text object will free @param other */
  void take(Text& other);

  /** make a copy of @param other. If other points to the same memory as this ... we might screw up */
  void copy(TextKey other);
private:
  /** relinquish ownership, which entails also forgetting the content. If used wrongly this will leak memory. */
  void release(){
    ptr=nullptr;
  }
public:
  /** discard==free internal content (if any) and null the internal pointer (to prevent use-after-free) */
  void clear() noexcept override;

  /** a core part of a parser is to take substrings, this does just that part of parsing. It never retains the content it manipulates, it creates copies that it hands to you. */
  class Chunker:public Span {
    Cstr base;
  public:
    Chunker (const char*start);
    /** pass back a copy of the substring defined by the cutter, and move the cutter @param leap past that */
    Text operator() (unsigned leap);
  };


}; // class TextPointer

#endif // TEXTPOINTER_H
