#ifndef STOREDLABEL_H
#define STOREDLABEL_H

#include "stored.h"

class StoredLabel : public Stored {
public:
  StoredLabel(Storable &node, const TextValue  &fallback = TextValue());
  void setDefault(const TextValue  &deftext);
  /**pointer to storage, not safe to use to manipulate it,*/
  TextKey c_str() const;
  /** your own self-deleting copy of the storage */
  TextValue toString() const;
  /** synatctic sugar for toString() */
  operator TextValue() const {
    return toString();
  }
  /** @returns whether the image of this label has zero length */
  bool isTrivial() const;

  void operator =(const StoredLabel &other);
  bool operator ==(const StoredLabel &other) const;
  void operator =(const TextValue  &zs);
  bool operator ==(const TextValue  &zs) const;
  void operator =(TextKey zs);
  bool operator ==(TextKey zs) const;

  /** calls the given slot with this.c_str() as its argument */
  void applyTo(sigc::slot<void, TextKey> slotty);
  /** on a change to the value will call applyTo with the given slot */
  sigc::connection onChange(sigc::slot<void, TextKey> slotty);
  /** a slot that will set the value of this */
  sigc::slot<void, TextKey> setter();
}; // class StoredLabel

#endif // STOREDLABEL_H
