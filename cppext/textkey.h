#ifndef TEXTKEY_H
#define TEXTKEY_H

/** marker for internal tag for text */
typedef const char* TextKey;

/** a token string is nontrivial if the pointer is not null and the first char is not the null terminator*/
bool nonTrivial(const TextKey t);
/** compare for equiality, with rational behavior when pointer is null */
bool same(const TextKey a,const TextKey b);
/** wraps strtod() to regularize use of its 2nd parameter.
 * @param impure if not null gets set to whether the numerical part of the string was followed by more text.*/
double toDouble(const TextKey rawText, bool* impure);  //apply wherever strtod() is used

#endif // TEXTKEY_H
