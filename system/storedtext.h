#pragma once
#include "stored.h"

/** a Stored piece of text.
*/
class StoredText : public Stored {
public:
  StoredText(Storable &node, const TextValue  &fallback = TextValue());
  void setDefault(const TextValue  &deftext);
  /**pointer to storage, not safe to use to manipulate it,*/
  TextKey c_str() const;
  /** your own self-deleting copy of the storage.
   * IE manipuating this does not affect the stored value.  */
  Text toString() const;
  /** syntactic sugar for toString() */
  operator TextValue() const {
    return toString();
  }
  /** @returns whether the image of this label has zero length */
  bool isTrivial() const;

  /** copy value from ... */
  void operator =(const StoredText &other);
  void operator =(const TextValue  &zs);
  void operator =(TextKey zs);
  /** convert integer to text and set this to that image */
  void operator =(int value);

  /** compare values, ignore names */
  bool operator ==(const StoredText &other) const;
  bool operator ==(const TextValue  &zs) const;
  bool operator ==(TextKey zs) const;

  /** calls the given slot with this.c_str() as its argument */
  void applyTo(sigc::slot<void, TextKey> slotty);
  /** on a change to the value will call applyTo with the given slot */
  sigc::connection onChange(sigc::slot<void, TextKey> slotty);
  /** a slot that will set the value of this */
  sigc::slot<void, TextKey> setter();
};
