#ifndef DELIMITEDINPUTSTREAM_H
#define DELIMITEDINPUTSTREAM_H

#include <istream>
#include <glibmm/ustring.h>


/**
  @class DelimitedInputStream is used to parse comma separated value files.
  values are pulled by calls.
  This file is gutted to just deal with our stripchart history, need to work on tokenstream and use shared code with that here.
*/
class DelimitedInputStream {

protected:
  gunichar separator;

  /** discards whitespace
@returns whether stream is still good */
  bool trimTrailing();
  /** discards whitespace
@returns whether stream is still good, inspect dstate for further details */
  bool trimLeading();
public:
  std::istream &s;
  int lineCount;//for debug
protected:
  /** state bits for treatment as single line ender.*/
  enum DIS_state {EndOfLine,StartOfField,EmptyField} dstate;
  //todo:2 selectable separator and quoting rules.
public:
  DelimitedInputStream(std::istream &s);
  /** @returns whether not at end of line */
  bool hasMoreFields();
  /** @returns whether not at end of file */
  bool hasMoreLines();
  /** parses optionally? quoted string */
  bool get(Glib::ustring &text);

  /** remove white space and parse content until white or separator encountered*/
  bool get(int &val);
  bool get(float &val);
  bool get(double &val);
  /** discard rest of line
@returns whether a normal end of line was encountered*/
  bool endl();
  //  /**marker for an endl that separates differently shaped blocks in the same file.*/
  //  bool gs();//"group separator"

};

#endif // DELIMITEDINPUTSTREAM_H
