#ifndef ZSTRING_H
#define ZSTRING_H

/** make a char * act like a class,
 *  protects against passing nulls to std lib str* functions treats them like a zero length string.
 *  N.B.: this is a syntactically fancier version of textpointer.
 *
 *  todo: check if 'this isnull' in methods so that a null Zstring * survives.
 *
 */
class Zstring {
protected: //
  char *str;
  bool owned;
  inline bool notNull() const {
    return this!=nullptr && str!=nullptr;
  }

public:
  /** if @param is makecopy then @see strdup is called and this class will free-on-delete */
  Zstring(char *str, bool makeCopy = false);
  /** allocate len+1 bytes and clear it, remember that we did the allocation and free on delete.*/
  Zstring(unsigned len = 0);
  ~Zstring();
  /** @returns string length a.k.a. offset in buffer of first null.
   *  return type is int rather than size_t due to statistical analysis of how often a cast was used in my code. */
  int len() const;
  char *chr(int chr) const;

  /** @returns strcmp nullptr is same as empty string */
  int cmp(const char *rhs) const;
  /** @see cmp */
  int cmp(const Zstring &rhs) const;

  /** @returns true if both are null or trivial else return cmp==0 */
  bool operator == (const char *rhs) const;
  bool operator == (const Zstring &rhs) const;

  /** copies content of the other, is owner of that copy.
   *  @returns this */
  Zstring &copy(const Zstring &other);
  /** assignment makes a copy */
  Zstring & operator = (const Zstring &other){
    return copy(other);
  }

  /** syntactic sugar for use in replacing std::string and similar classes
  *  The value returned here should not be retained, it might get freed */
  char *c_str() const;

  /** forgets present content, deleting it if owned.*/
  void clear();
  /** for when you know this was pointing to a malloc'd string and you wish to free it. Nulls internal pointer to normalize use-after-free's.
   *  N.B.: you probably want to use @see clear() */
  void free();
}; // class Zstring

#endif // ZSTRING_H
