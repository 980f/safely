#ifndef TEXTKEY_H
#define TEXTKEY_H

/** marker for internal tag for text */
typedef const char* TextKey;

//////////// inline stringy stuff /////////////
#define ERRLOC(moretext) __FILE__ "::" moretext

// some unicode chars frequently used by us mathematical types:
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

/** @returns whether the @param string exists and is not just a null terminator */
inline bool isTrivial(const char *string){
  return string==nullptr || *string==0;
}

/** a token string is nontrivial if the pointer is not null and the first char is not the null terminator*/
bool nonTrivial(const TextKey t);
/** compare for equality, with rational behavior when either pointer is null */
bool same(const TextKey a,const TextKey b);

/** wraps strtod() to regularize use of its 2nd parameter.
 * @param impure if not null gets set to whether the numerical part of the string was followed by more text.*/
double toDouble(const TextKey rawText, bool* impure);  //apply wherever strtod() is used

#endif // TEXTKEY_H
