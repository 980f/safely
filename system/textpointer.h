#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "textkey.h"

class TextPointer {
private:
  const char *ptr;
public:
  /** creates an 'empty' one */
  TextPointer();

  /** makes a copy of the @param given content */
  TextPointer(const char *ptr);

  /** deletes its copy of the content copied by the constructor */
  ~TextPointer();

  /** deletes present content (if any) and copies @param ptr content (if any) */
  const char *operator =(const char *ptr);

  /** @returns pointer member  */
  operator const char *() const;

  /** @returns whether content is non-existent or trivial */
  bool empty () const;

  /** @returns whether @param other exactly matches this' content */
  bool is(const char *other) const;

  /** needed by changed() template function */
  bool operator !=(const char *other){
    return !is(other);
  }
  /** needed by changed() template function if we kill the != one*/
  bool operator ==(const char *other){
    return is(other);
  }

  /** @returns whether this' content matches @param other for all of the chars of other */
  bool startsWith(const char *other) const;

  /** discard internal content (if any) */
  void clear();

  /** @returns internal pointer suitably typecast for use by old str' functions which ask for char*'s even though they don't alter the chars.
    * NB: avoid use of this as it allows you to alter const data. */
  char *buffer() const;
};

#endif // TEXTPOINTER_H
