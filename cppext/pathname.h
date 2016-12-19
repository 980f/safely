#ifndef PATHNAME_H
#define PATHNAME_H


#include "charformatter.h"
#include "textkey.h"  //most path elements are fixed text
//#include "eztypes.h"  //fixed elements of paths

/** build a segment name in an overflow protected buffer, such as for a filename, but that will be an embellishment of this.
 *  this class does not allocate data, you give it via pointer and allocation size.
 * Compared to its Glib partner this one does not implement a default 'root', run the app in chroot with externally set link to tmp
 */
class Pathname : public CharFormatter {
  /** single seperator path*/
  const char seperator;

public:
  Pathname(Indexer<char> &other,char slasher='/');
  virtual ~Pathname();

  /** reset to be just the seperator*/
  Pathname &root();

    /** removes the last path element, similar to the dirname unix command */
  Pathname &parent(void);

  /** @returns this. Adds another path element preceding it with seperator if needed*/
  Pathname &addChild(const TextKey &s, bool escapeit = false);

    /** make sure string ends with given token*/
  Pathname &assure(char token);
//  /** add a dot if one isn't present then add given text.*/
//  Pathname &ext(const Ustring &s,bool escapeit = false);
//  FileName&operator = (const FileName &other);
//  /** a value that if passed into FileName() will give the same value as this has now*/
//  Glib::ustring relative() const;


  void addNode(Cstr node);
};

#endif // PATHNAME_H
