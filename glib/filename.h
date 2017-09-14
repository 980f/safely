//(C) 2017 Andrew Heilveil
#ifndef FILENAME_H
#define FILENAME_H

#include <glibmm/ustring.h>

/**
  * somewhere else, typically where the main() resides, you must declare the absolute root:
  * const char *FileName::root = "/nexqc/";
  */
class FileName: public Glib::ustring {
public:
  static const Glib::ustring root;
public:
  FileName();
  FileName(const Glib::ustring &simple);

  /** removes the last path element, similar to the dirname unix command */
  FileName &dirname(void);
  /** make sure string ends with given token*/
  FileName &assure(char token);
  /** misnamed, adds another path element preceding it with / if needed*/
  FileName &folder(const Glib::ustring &s,bool escapeit=false);
  /** add a dot if one isn't present then add given text.*/
  FileName &ext(const Glib::ustring &s,bool escapeit=false);
//  FileName&operator = (const FileName &other);
  /** reset to be just our globally enforced root directory*/
  FileName &slash();
  /** a value that if passed into FileName() will give the same value as this has now*/
  Glib::ustring relative() const;
public:
  gunichar lastChar() const;
};

/** useful for managing a recursively named file set.*/
struct NameStacker {
  FileName &path;
  FileName oldpath;
  bool escapeit; //for passing escaper option through NameStacker constructor
public:
  /** append something now, remove it when destructed. */
  NameStacker(FileName &path, bool escapeit = true);
  NameStacker(FileName &path, const Glib::ustring &pushsome, bool escapeit = true);
  /** append something now, remove it when destructed. */
  NameStacker(NameStacker&path);
  NameStacker(NameStacker&path, const Glib::ustring &pushsome);
  /** possibly ill-advised accessor*/
  operator FileName &();
  /** remove what was added by constructor */
  ~NameStacker();
};


//todo: wrap the following (remainder of this file!) inside FileName class, where it all used to be, so that we didn't have such broad linkage to glib/gio and these members themselves were'nt really supposed to be global in use.
#include "glibmm/refptr.h"
#include "giomm/file.h"

//todo: why use a RefPtr (with its annoying usage syntax) when the object's lifetime is the same as the application's?
extern Glib::RefPtr< Gio::File > nexqcRoot;
extern Glib::RefPtr< Gio::File > nexqcExternal;

/** some places didn't like this inlined:
path under our project root which mounts removable media or an nfs.
*/
const char External[]="external";

#endif // FILENAME_H
