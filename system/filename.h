#ifndef FILENAME_H
#define FILENAME_H

#include "segmentedname.h"
#include "utf8.h"

/** if you want a project root, I suggest running the application in a chroot.
 *
 */
class FileName : public SegmentedName {
public:
  FileName();
  FileName(TextKey simple);
  FileName(FileName &other);

  /** removes the last path element, similar to the dirname unix command */
  FileName &dirname(void);
  /** make sure string ends with given token*/
  FileName &assure(char token);
  /** misnamed, adds another path element preceding it with / if needed*/
  FileName &folder(const Text &s,bool escapeit = false);
  /** add a dot if one isn't present then add given text.*/
  FileName &ext(const Text &s,bool escapeit = false);
//  FileName&operator = (const FileName &other);
/** reset to be just our globally enforced root directory*/
  FileName &slash();

public:
  Unichar lastChar() const;
}; // class FileName


/** useful for managing a recursively named file set.*/
class NameStacker {
  FileName &path;
  FileName oldpath;
  bool escapeit; //for passing escaper option through NameStacker constructor
public:
  /** append something now, remove it when destructed. */
  NameStacker(FileName &path, bool escapeit = true);
  NameStacker(FileName &path, const Text &pushsome, bool escapeit = true);
  /** append something now, remove it when destructed. */
  NameStacker(NameStacker&path);
  NameStacker(NameStacker&path, const Text &pushsome);
  /** possibly ill-advised accessor*/
  operator FileName &();
  /** remove what was added by constructor */
  ~NameStacker();
};

#endif // FILENAME_H
