#ifndef FILENAME_H
#define FILENAME_H

#include "segmentedname.h"
#include "pathparser.h"

/** mates segmented name and pathparser to provide file naming utility
 *
 * this approach maintains the path as an array of pieces, not assembled into a proper string until pack is called, which generates an independent chunk of text.
 */
class FileName : public SegmentedName {
public:
  FileName();
  /** parses into first path element*/
  FileName(TextKey simple);
  /** parses, which copies */
  FileName(const Text &simple);
  /** copies elements */
  FileName(const FileName &other);

  /** removes the last path element, similar to the dirname unix command
   * @returns this as reference */
  FileName &dirname(void);
//  /** make sure string ends with given token. @returns this */
//  FileName &assure(char token);
  /** parses appending.
 @returns this as reference */
  FileName &folder(const Text &s,bool escapeit = false);
  /** modifies last path member, add a dot if one isn't present then add given text. creates one if empty
 @returns this as reference */
  FileName &ext(const Text &s,bool escapeit = false);
/** reset to be just our globally enforced root directory.
@returns this as reference */
  FileName &erase();

public:
  bool lastChar(char isit) const;
  Text pack(PathParser::Brackets bracket);
  Text pack();
}; // class FileName


/** useful for managing a recursively named file set.*/
class NameStacker {
  FileName &path;
  unsigned mark;
  bool escapeit; //for passing escaper option through NameStacker constructor
public:
  NameStacker(FileName &path, bool escapeit = true);
  NameStacker(FileName &path, const Text &pushsome, bool escapeit = true);
//  NameStacker(NameStacker&path);
//  NameStacker(NameStacker&path, const Text &pushsome);
  /** remove what was added by constructor */
  ~NameStacker();
  /** access to embedded @see FileName::pack */
  Text pack(const PathParser::Brackets &bracket);
};

#endif // FILENAME_H
