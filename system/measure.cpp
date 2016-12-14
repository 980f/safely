#include "measure.h"
#include "numberformatter.h"

Measure::Measure(Storable &node):
  Stored(node),
  ConnectChild(value),
  ConnectChild(uom),
  ConnectChild(precision, Nan) {
}

Glib::ustring Measure::format(double number, bool addone) const {
  if(isSignal(number)) {
    return "---";
  }
  if(isNan(precision)) {
    return Glib::ustring::format(number, " ", uom.toString());
  }
  NumberFormatter nf(false, precision, uom);
  return nf.format(number, addone);
}

Formatter Measure::formatter() const {
  return sigc::bind(MyHandler(Measure::format), false);
}

NamedMeasure::NamedMeasure(Storable &node):
  Measure(node),
  ConnectChild(name) {
}
