#ifndef STOREDLABEL_H
#define STOREDLABEL_H

#include "storable.h"

class StoredLabel : public Stored {
public:
  StoredLabel(Storable &node, const TextValue  &fallback = TextValue());
  void setDefault(const TextValue  &deftext);
  const char *c_str() const;
  //this cast operator created "ambiguous overload" due to the various operator == methods.
  //  operator const char *() const{
  //    return c_str();
  //  }
  TextValue toString() const;
  operator TextValue() const {
    return toString();
  }

  bool isTrivial() const;
  void operator =(const StoredLabel &other);
  bool operator ==(const StoredLabel &other) const;
  void operator =(const TextValue  &zs);
  bool operator ==(const TextValue  &zs) const;
  void operator =(const char *zs);
  bool operator ==(const char *zs) const;

  /** calls the given slot with this.toString() as its argument */
  void applyTo(sigc::slot<void, const char *> slotty);
  /** on a change to the value will call applyTo with the given slot */
  sigc::connection onChange(sigc::slot<void, const char *> slotty);
  /** a slot that will set the value of this */
  sigc::slot<void, const char *> setter();
}; // class StoredLabel

#endif // STOREDLABEL_H
