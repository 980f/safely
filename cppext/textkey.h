#pragma once

/** marker for internal tag for const text, especially useful in getting 'const' onto the correct place
 *
 * There is no check to see if the pointed to memory is actually ram rather than rom (or MMU read-only section).
 * As such some of the routines may return a pointer into a string that you then later free. You must assume that the lifetime of any returned value is the same as that of the argument.
 */
using TextKey = const char *;

/** for converting text as bytes to bytes */
inline const unsigned char *raw(const char *ptr) {
  return reinterpret_cast<const unsigned char *>(ptr);
}

/** add 1 to reserve space for null terminator
 * this is a marker for '1+' or '+1'
 */
inline unsigned Zguard(unsigned allocation) {
  return 1 + allocation;
}


//////////// inline stringy stuff /////////////
#define ERRLOC(moretext) __FILE__ "::" moretext

// some Unicode chars frequently used by us mathematical types:
#define Degree "\u00b0"
#define DegreeC Degree "C"
#define Sup2   "\u00B2"
//Alpha was used as a variable name in some sacred code :(
#define AlphaChar "\u03b1"
#define Beta  "\u03b2"
#define Gamma "\u03b3"
#define Delta "\u03b4"

#define Mu    "\u03bc"
#define Sigma "\u03c3"
//feel free to add in other greek characters, like psi and phi and pi!

/** @returns whether the @param string is nullptr or just points to a null terminator */
bool isTrivial(TextKey key);

/** @returns whether @param  key is not null and the first char is not the null terminator*/
bool nonTrivial(TextKey key);

/** alters @param t to point to a zero length string if it presently is the null pointer */
void denull(TextKey &t);

/** compare for equality, with rational behavior when either pointer is null */
bool same(TextKey a, TextKey b);

/** wraps strtod() to regularize use of its 2nd parameter.
 * @param impure if not null gets set to whether the numerical part of the string was followed by more text.*/
double toDouble(TextKey rawText, bool *impure = nullptr);

unsigned toIndex(TextKey rawText, bool *impure = nullptr);
