#ifndef CSTR_H
#define CSTR_H

#include "textkey.h" //for its utility functions, could migrate those here
#include "index.h"  //for string search results

/** yet another attempt at safe use of standard C lib str functions.
 *
 * This class wraps a pointer and null checks all uses, vs letting str*() lib functions seg fault.
 * only str* functions which do NOT alter the string should be wrapped here.
 *
 * While most of the functions are readily inlined we are putting them in a cpp file and will trust LTO (link time optimisation) to figure that out.
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
   *  not using size_t due to textual analysis of frequency of casts.*/
  unsigned length() const noexcept;

  /** @returns whether last character exists and is same as @param isit. an empty string will positively match char(0) */
  bool endsWith(char isit) const noexcept;

  /** @returns whether @param other exactly matches this' content */
  bool is(TextKey other) const  noexcept;

  char operator [](const Index &index) const noexcept;

  /** needed by changed() template function */
  bool operator !=(TextKey other) const  noexcept{
    return !is(other);
  }

  /** needed by changed() template function if we kill the != one*/
  bool operator ==(TextKey other) const  noexcept{
    return is(other);
  }

  /** @returns strcmp treating nullptr's as same as "" */
  int cmp(TextKey rhs) const noexcept;

  /** @returns whether this' content matches @param other for all of the chars of other */
  bool startsWith(TextKey other) const noexcept;

  /** @returns position of first character in this string which matches ch. */
  Index index(char ch) const noexcept;

  /** @returns position of last character in this string which matches ch. */
  Index rindex(char ch) const noexcept;

  /** @returns pointer to first character in this string which matches ch.
   * If you know the origin of the string this is pointing to you can dare to const_cast<char *> the value here. */
  const char *chr(int chr) const noexcept;

  /** @returns pointer to first character in this string which matches ch. @see chr() */
  const char *rchr(int chr) const noexcept;

  /** forget the target */
  virtual void clear() noexcept;

  /** marker for tedious syntax const_cast<char *>()
   * this should only be used when passing the pointer to old stdlib functions, and only when you have verified the string is null terminated.
   */
  static char *violate(TextKey violatus) noexcept{
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
