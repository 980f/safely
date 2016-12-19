#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "textkey.h"

/** manages a mallocated chunk of text, without knowing its allocated size.
 *  it relies upon char * arguments to its methods to be properly null terminated.
 *  it does NOT rely upon such arguments beyond the execution of any method, this class was first used to keep copies of the content
 * of strings that might be de-allocated by future actions.
 *  it should perhaps not be in a library with 'safely' in its name.
 * For a more paranoid version @see Zstring.
 */
class TextPointer {
private:
  TextKey ptr;
public:
  /** creates an 'empty' one */
  TextPointer();

  /** makes a copy of the @param given content */
  TextPointer(TextKey ptr);

  /** deletes its copy of the content copied by the constructor */
  ~TextPointer();

  /** deletes present content (if any) and copies @param ptr content (if any) */
  TextKey operator =(TextKey ptr);

  /** @returns pointer member  */
  operator TextKey() const;

  /** @returns whether content is non-existent or trivial */
  bool empty () const;

  /** @returns length, 0 if ptr is null.
   *  not using size_t due to textual analysis of frequency of casts. Using signed int as in enough places we use -1 as a signal to go find the length.*/
  int length() const;

  /** @returns whether @param other exactly matches this' content */
  bool is(TextKey other) const;

  /** needed by changed() template function */
  bool operator !=(TextKey other) const {
    return !is(other);
  }

  /** needed by changed() template function if we kill the != one*/
  bool operator ==(TextKey other) const {
    return is(other);
  }

  /** @returns whether this' content matches @param other for all of the chars of other */
  bool startsWith(TextKey other) const;

  /** discard internal content (if any) */
  void clear();

  /** @returns internal pointer suitably typecast for use by old str' functions which ask for non-const char*'s even though they
   * don't alter the chars.
   */
  char *buffer() const;
}; // class TextPointer

#endif // TEXTPOINTER_H
