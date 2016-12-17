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
  bool owned;
public:
  /** if @param is makecopy then @see strdup is called and this class will free-on-delete */
  Zstring(char *str, bool makeCopy = false);
  /** allocate len+1 bytes and clear it, remember that we did the allocation and free on delete.*/
  Zstring(unsigned len = 0);
  ~Zstring();
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

  /** for when you know this was pointing to a malloc'd string and you wish to free it. Nulls internal pointer to prevent use-after-free's */
  void free();
}; // class Zstring

#endif // ZSTRING_H
