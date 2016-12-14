#ifndef TEXTKEY_H
#define TEXTKEY_H

/** marker for internal tag for text */
typedef const char* TextKey;

bool nonTrivial(TextKey t);
void denull(TextKey &t);
bool same(TextKey a,TextKey b);
double toDouble(TextKey rawText, bool* impure);  //apply wherever strtod() is used

#endif // TEXTKEY_H
