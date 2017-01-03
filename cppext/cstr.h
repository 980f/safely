#ifndef CSTR_H
#define CSTR_H

#include "textkey.h" //for its utility functions, could migrate those here

/** yet another attempt at safe use of standard C lib str functions.
 *
 * This class wraps a pointer and null checks all uses, vs letting str*() lib functions seg fault.
 * only str* functions which do NOT alter the string should be wrapped here.
 */

class Cstr {
protected://we are a base class
  const char *ptr;
  /** pointer to a shared null char. */
  static constexpr const char *const emptyString = "";

public:
  Cstr();
  Cstr(TextKey target);

  //virtual destructor as this is a base for classes which may do smart things with the pointer on destruction.
  virtual ~Cstr();
  /** change internal pointer */
  virtual TextKey operator =(TextKey ptr);

  /** @returns pointer member, allowing you to bypass the checks of this class.  */
  operator TextKey() const;

  /** @returns pointer member, allowing you to bypass the checks of this class.
   * Name is from/for replacing glib::ustring and std::string */
  TextKey c_str() const;

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

  /** @returns pointer to first character in this string which matches ch.
   * If you know the origin of the string this is pointing to you can dare to const_cast<char *> the value here. */
  const char *chr(int chr) const;

  /** @returns pointer to first character in this string which matches ch. @see chr() */
  const char *rchr(int chr) const;

  /** forget the target */
  virtual void clear();

  /** marker for tedious syntax const_cast<char *>()
   * this should only be used when passing the pointer to old stdlib functions, and only when you have verified the string is null terminated.
   */
  static char *violate(TextKey violatus){
    return const_cast<char *>(violatus);
  }

  /** marker for tedious syntax const_cast<char *>()
   * this should only be used when passing the pointer to old stdlib functions, and only when you have verified the string is null terminated.
   */
  char *violated(){
    return const_cast<char *>(ptr);
  }

}; // class Cstr

#endif // CSTR_H
