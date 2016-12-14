#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/** abstract number printer, convert number into text. */
typedef sigc::slot < Glib::ustring, double /*value*/ > Formatter;

struct NumberFormatter {
  bool fp; //else fixed decimals
  int precision;//exposed for dynamic control
  Glib::ustring postfix;
  NumberFormatter(bool fp, int precision, const Glib::ustring &postfix = ""); //else fixed decimals

  //right alignment is only available for fp==false, force fixed number of decimals with trailing zeroes.
  Glib::ustring format(double value,bool addone=false) const;
};


#endif // NUMBERFORMATTER_H
