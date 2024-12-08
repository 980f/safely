#include "measure.h"

#include <ustring.h>

#include "numberformatter.h"
// #include "uicore.h"

Measure::Measure(Storable &node) :
  Stored(node),
  ConnectChild(value),
  ConnectChild(uom),
  ConnectChild(precision, Nan){
}

Ustring Measure::format(double number, bool addone) const {
  if(isSignal(number)) {
    return "---";
  }
  if(isNan(precision)) {
    return Ustring::format(number, " ", uom.toString());
  }
  NumberTextifier nf(false, precision, uom);
  return nf.format(number, addone);
}

Formatter Measure::formatter() const {
  return sigc::bind(MyHandler(Measure::format), false);
}

NamedMeasure::NamedMeasure(Storable &node) :
  Measure(node),
  ConnectChild(name){
}
