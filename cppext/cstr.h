#ifndef CSTR_H
#define CSTR_H

#include "textkey.h" //for its utility functions, could migrate those here


/** yet another attempt at safe use of str library.
 * This class wraps a pointer and null checks all uses, vs letting str*() lib functions seg fault.
 */

class Cstr {
protected://we are a base class
  char *ptr;
  /** pointer to a shared null char. */
  static constexpr const char *const emptyString = "";

public:
  Cstr();
  Cstr(char * target);
  //virtual destructor as this is a base for classes which may do smart things with the pointer on destruction.
  virtual ~Cstr();
  /** change internal pointer */
  virtual TextKey operator =(TextKey ptr);

//  /** @returns pointer member  */
//  operator char *() const;

  /** @returns pointer member  */
  operator const char *() const;

  /** @returns whether content is non-existent or trivial */
  bool empty() const;

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

  /** @returns strcmp treating nullptr's as same as "" */
  int cmp(TextKey rhs) const;

  /** @returns whether this' content matches @param other for all of the chars of other */
  bool startsWith(TextKey other) const;

  /** @returns position of first character in this string which matches ch. */
  int index(char ch) const;

  /** @returns position of last character in this string which matches ch. */
  int rindex(char ch) const;

  /** @returns pointer to first character in this string which matches ch. */
  char *chr(int chr) const;

  /** @returns pointer to first character in this string which matches ch. */
  char *rchr(int chr) const;


  /** forget the target */
  virtual void clear();

}; // class Cstr

#endif // CSTR_H
