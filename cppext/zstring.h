#ifndef ZSTRING_H
#define ZSTRING_H

/** make a char * act like a class, localize uses of str* functions for inspection.
 *  most especially it includes null checks and treats them like a zero length string
 *  N.B.: does not copy the data on construction or assignment, see textpointer for a safer class.
 *
 *  todo: check if this isnull in methods so that a null Zstring * survives.
 *
 */
class Zstring {
protected: //
  char *str;
public:
  Zstring(char *str);
  int len() const;
  char *chr(int chr) const;

  /** @returns strcmp nullptr is same as empty string */
  int cmp(const char *rhs) const;
  /** @see cmp */
  int cmp(const Zstring &rhs) const;

  /** @returns true if both are null or trivial else return cmp==0 */
  bool operator == (const char *rhs) const;
  bool operator == (const Zstring &rhs) const;


  //syntactic sugar for use in replacing std::string and similar classes
  char *cstr() const;

  //for when you know this was pointing to a malloc'd string and you wish to free it
  void free();
}; // class Zstring

#endif // ZSTRING_H
