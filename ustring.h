#ifndef USTRING_H
#define USTRING_H

/** wrapper for common subset of classes like Glib::ustring and std::string
 *  the implementation file here is a simple one, but may integrate with UTF* processing classes.
 */
class Ustring {
public:
  Ustring();
  Ustring(const char *content);
  Ustring(const Ustring &other);
  /** pointer to raw storage, presumed to be null terminated */
  char * cstr();
};

/** replace Glib gunichar*/
typedef u32 Unichar;
#endif // USTRING_H
