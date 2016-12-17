#ifndef MEASURE_H
#define MEASURE_H

#include "storable.h"
#include "storednumeric.h"

////////////////////////

class Measure : public Stored {
public:
  operator double() const {
    return value.native();
  }

  StoredReal value;
  StoredLabel uom; //croassapi requires an enum here, but empirical does not so we have to hand apply the enum at places of use.
  /** number of decimal places, NaN if no precision */
  StoredReal precision;

  Measure(Storable &node);
  Ustring format(double number, bool addone = false) const;
  /** @returns a slot for calling format() with just the number */
  Formatter formatter() const;
}; // class Measure

class NamedMeasure : public Measure {
public:
  StoredLabel name;//as nice as it would be to rename this to 'label' we would have upgrade issues

  NamedMeasure(Storable &node);
};

#endif // MEASURE_H
