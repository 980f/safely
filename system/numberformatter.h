#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

#include "safely.h"
#include "sigcuser.h"
#include "ustring.h"

/** abstract number printer, convert number into text. */
typedef sigc::slot< Ustring, double /*value*/ > Formatter;

struct NumberFormatter {
  bool fp; //else fixed decimals
  int precision;//exposed for dynamic control
  Ustring postfix;
  NumberFormatter(bool fp, int precision, const Ustring &postfix = ""); //else fixed decimals

  //right alignment is only available for fp==false, force fixed number of decimals with trailing zeroes.
  Ustring format(double value,bool addone = false) const;
};


#endif // NUMBERFORMATTER_H
