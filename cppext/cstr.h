#ifndef CSTR_H
#define CSTR_H

#include "textkey.h" //for its utility functions, could migrate those here
#include "index.h"  //for string search results

/** yet another attempt at safe use of standard C lib str functions.
 * Assiduous use will make it hard for you to make use-after-free bugs.
 *
 * This class wraps a pointer and null checks all uses, vs letting str*() lib functions seg fault.
 * NB: only str* functions which do NOT alter the string should be wrapped here.
 * When data is free'd the pointer is nulled so instead of use-after-free you get a trivial string.
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
  Cstr(TextKey target);//# we desire implicit conversions
  Cstr(unsigned char *target);//# we desire implicit conversions

  //virtual destructor as this is a base for classes which may do smart things with the pointer on destruction.
  virtual ~Cstr() = default;//we never take ownership of ptr, see class Text for such a beast.
  /** change internal pointer */
  virtual TextKey operator =(TextKey ptr);//# we desire passthrough on argument

  /** @returns pointer member, allowing you to bypass the checks of this class.  */
  operator TextKey() const;

  /** @returns pointer member, allowing you to bypass the checks of this class.
   * Name is from/for replacing glib::ustring and std::string */
  TextKey c_str() const;

  /** as byte vs human readable character */
  const unsigned char*raw() const;

  const char *notNull() const;

  /** @returns the pointer if the string length is >0 else returns nullptr.*/
  const char *nullIfEmpty() const;

  /** @returns whether content is non-existent or trivial */
  bool empty() const;

  /** @returns length, 0 if ptr is null.
   *  not using size_t due to textual analysis of frequency of casts.*/
  unsigned length() const noexcept;

  /** @returns whether last character exists and is same as @param isit. an empty string will positively match char(0) */
  bool endsWith(char isit) const noexcept;

  /** @returns whether @param other exactly matches this' content, with nullptr matching the same or "" */
  bool is(TextKey other) const noexcept;

  char operator [](const Index &index) const noexcept;

  char at(const Index &index) const noexcept;

  /** @returns whether the string was modified. NB: this violates constness of the original string, user beware*/
  bool setAt(const Index&index,char see) const noexcept;

  /** needed by changed() template function */
  bool operator !=(TextKey other) const noexcept {
    return !is(other);
  }

  /** needed by changed() template function if we kill the != one */
  bool operator ==(TextKey other) const noexcept {
    return is(other);
  }

  /** @returns strcmp treating nullptr's as same as "" */
  int cmp(TextKey rhs) const noexcept;

  /** @returns whether this' content matches @param other for all of the chars of other */
  bool startsWith(TextKey other) const noexcept;

  bool startsWith(char ch) const noexcept;

  /** @returns position of first character in this string which matches ch. */
  Index index(char ch) const noexcept;

  /** @returns position of last character in this string which matches ch. */
  Index rindex(char ch) const noexcept;

  /** @returns pointer to first character in this string which matches ch.
   * If you know the origin of the string this is pointing to you can dare to const_cast<char *> the value here. */
  const char *chr(int chr) const noexcept;

  /** @returns pointer to last character in this string which matches ch. @see chr() */
  const char *rchr(int chr) const noexcept;

  /** @returns an index that if nulled will remove useless '0' digits, and maybe a radix point. This is dumb and presumes the string is a reasonable representation of a number. */
  Index trailingZeroes() const;

  /** strtod */
  double asNumber(Cstr *tail = nullptr) const noexcept;

  /** for bool: 1,0,true, false have definite values.
   * to distinguish onNull from the same value being parsed inspect units.
   */
  template<typename Numeric> Numeric cvt(Numeric onNull, Cstr *units = nullptr) const noexcept;
  /** forget the target */
  virtual void clear() noexcept;

  /** marker for tedious syntax const_cast<char *>()
   * this should only be used when passing the pointer to old stdlib functions, and only when you have verified the string is null terminated.
   */
  static char *violate(TextKey violatus) noexcept {
    return const_cast<char *>(violatus);
  }

  /** marker for tedious syntax const_cast<char *>()
   * this should only be used when passing the pointer to old stdlib functions, and only when you have verified the string is null terminated.
   */
  char *violated(){
    return const_cast<char *>(ptr);
  }

  unsigned char *casted(){
    return const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(ptr));
  }

}; // class Cstr

//versions implemented in cstr.cpp. You should probably add others here if the type is intrinsic or already known to this module.
template<> bool Cstr::cvt(bool onNull, Cstr *units) const noexcept;
template<> long Cstr::cvt(long onNull, Cstr *units) const noexcept;
template<> unsigned Cstr::cvt(unsigned onNull, Cstr *units) const noexcept;
template<> int Cstr::cvt(int onNull, Cstr *units) const noexcept;
template<> double Cstr::cvt(double onNull, Cstr *units) const noexcept;

#endif // CSTR_H
