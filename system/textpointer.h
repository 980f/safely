#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "cstr.h"

/** adds strdup'ing to Cstr functionality, i.e. makes a copy on construction and assignement vs Cstr which just looks at someone else's allocated memory.
 *
 *  This class unconditionally frees the data it points at when destroyed. IE it always owns what it points at.
 */
class Text : public Cstr {

public:
  /** creates an 'empty' one */
  Text();

  /** makes a copy of the @param given content if @param takeit is false, else presumes the caller is happy with this maintaining the lifetime.
   * IE: if takeit is true then this class just records the pointer, if false it makes a copy of the data and forgets the given pinter.
   * This class *always* frees the data it points at.

 */
  Text(TextKey ptr,bool takeit);

  /** makes a copy of the @param given content */
  Text(TextKey other);

  /** calloc's a block of data. */
  Text(unsigned size);

  /** take contents of @param other, hence other cannot be const as we null its pointer lest we get a double-free */
  Text(Text &other);

  /** make a copy of non-null-terminated subset of some string */
  Text(TextKey other,unsigned begin, unsigned end);

public:

  /** deletes its copy of the content copied or recorded by the constructor */
  virtual ~Text();

  /** useful for forcing a copy when constructing, the copy constructor is used for moving. */
  operator TextKey() const;

  /** deletes present content (if any) and copies @param ptr content (if any).
   * @returns the @param pointer, not a pointer to self or the copy made.
   */
  TextKey operator =(TextKey other) override;

  /** take ownership of a buffer, i.e. deleting this Text object will free @param other */
  void take(TextKey other);

  /** make a copy of @param other. If other points to the same memory as this ... we might screw up */
  void copy(TextKey other);

  /** discard==free internal content (if any) and null the internal pointer (to prevent use-after-free) */
  void clear() override;

}; // class TextPointer

#endif // TEXTPOINTER_H
