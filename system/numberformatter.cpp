#include <iomanip> //for setprecision
#include "numberformatter.h"

using namespace std;
using namespace Glib;

NumberFormatter::NumberFormatter(bool fp, int precision, const ustring &postfix):
  fp(fp),
  precision(precision),
  postfix(postfix) {
  //#nada
}

ustring NumberFormatter::format(double value,bool addone) const {
  if(fp) {
    return ustring::format(setprecision(precision+(addone?1:0)), value, " ", postfix);
  } else {
    return ustring::format(fixed, setprecision(precision+(addone?1:0)), value, " ", postfix);
  }
}
