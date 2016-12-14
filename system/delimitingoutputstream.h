#ifndef DELIMITINGOUTPUTSTREAM_H
#define DELIMITINGOUTPUTSTREAM_H

#include <ostream>
#ifndef NO_GLIB
#include <glibmm/ustring.h>
#endif

/**
@class DelimitingOutputStream is used to generate comma separated value files.
todo:2 use imbue'ing a locale to manage variations. e.g. make a windows compatible locale for outputting tfr files.
*/
class DelimitingOutputStream {
public:
  /** publicized so that struct generates can add line prefix and suffix without commas.*/
  std::ostream &os;
  bool linestarted;
  void separator();
  /** controls for endl(), not trusting platform setting*/
  const char *lineTerminator;
  //todo:2 selectable separator and quoting rules.
public:
  DelimitingOutputStream(std::ostream &os, bool withBom=false, bool crlfs=true);//defaulting to micro$ofts choice as that is who we target files at.
#ifndef NO_GLIB
  DelimitingOutputStream &put(const Glib::ustring &text);
#endif
  DelimitingOutputStream &put(const char *text);
  DelimitingOutputStream &put(int val);
  DelimitingOutputStream &put(float val,int sigfig=8);
  DelimitingOutputStream &put(double val,int sigfig=17);
  DelimitingOutputStream &endl();
  /** insert byte order mark if @param doit is true*/
  DelimitingOutputStream &bom(bool doit);
  /**marker for an endl that separates differently shaped blocks in the same file.*/
  DelimitingOutputStream &gs();//"group separator"
};

#endif // DELIMITINGOUTPUTSTREAM_H
