#ifndef TEXTKEY_H
#define TEXTKEY_H

/** marker for internal tag for text */
typedef const char* TextKey;

/** a token string is nontrivial if the pointer is not null and the first char is not the null terminator*/
bool nonTrivial(TextKey t);
//too dangerous, if user is that farked then this doesn't really help /** modifies @param t pointer to not be null*/
//void denull(TextKey &t);
bool same(TextKey a,TextKey b);
double toDouble(TextKey rawText, bool* impure);  //apply wherever strtod() is used

#endif // TEXTKEY_H
