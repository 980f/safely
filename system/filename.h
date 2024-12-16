#pragma once //"(C) 2017 Andrew Heilveil"

#include "dottedname.h"
#include "filenameconverter.h"
#include "pathparser.h"

/** a filename as an array of DottedNames, each of which is an array of text.
 * this approach maintains the path as an array of pieces, not assembled into a proper string until pack is called, which generates an independent chunk of text.
 * this has an advantage that intentional path separators aren't url encoded, only ones embedded in data appended to the path.
 */
class FileName : public Chain<DottedName> {
public:
  PathParser::Rules bracket;//outer rules
  FileName();
  /** parses into first path element*/
  FileName(TextKey simple);
  /** parses, which copies */
  FileName(const Text &simple);
  /** copies elements */
  FileName(const FileName &other);

  /** @returns whether filename is trivial. Note: "/" is trivial, we make it hard to pass 'whole filesystem' to anything */
  bool empty() const{
    return quantity()==0;
  }
  /** removes the last path element, similar to the dirname unix command
   * @returns this as reference */
  FileName &dirname();

  /** modifies last path member, add a dot if one isn't present then add given text. creates one if empty
 @returns this as reference */
  FileName &ext(const Text &s);
/** reset to be just our globally enforced root directory.
@returns this as reference */
  FileName &erase();

public:
  /** parses appending.
 @returns this as reference */
  FileName &parse(const char *rawpath);
  unsigned length(Converter &&cvt=FileNameConverter())const;
  /** assemble string to pass to normal code. @param cvt is for escaping special charaacters */
  Text pack(Converter &&cvt=FileNameConverter(), unsigned bytesNeeded=BadIndex);
}; // class FileName


/** useful for managing a recursively named file set.*/
class NameStacker {
  FileName &path;
  unsigned mark;
public:
  NameStacker(FileName &path);
  NameStacker(FileName &path, const Text &pushsome);
//  NameStacker(NameStacker&path);
//  NameStacker(NameStacker&path, const Text &pushsome);
  /** remove what was added by constructor */
  ~NameStacker();
  /** access to embedded @see FileName::pack */
  Text pack();
};
