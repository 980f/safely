#pragma once

#include "stored.h"

/** a Stored piece of text.
*/
class StoredLabel : public Stored {
public:

  StoredLabel(Storable&node, const char *fallback) ;
  StoredLabel(Storable &node, const TextValue &fallback = TextValue()) :StoredLabel(node,fallback.c_str()){}


  void setDefault(const char *deftext);
  void setDefault(const TextValue &deftext) {setDefault(deftext.c_str());}

  /** pointer to storage, not safe to use to manipulate it. */
  TextKey c_str() const;
  /** your own self-deleting copy of the storage.
   * IE manipulating this does not affect the stored value.  */
  Text toString() const;
  /** syntactic sugar for toString() */
  operator TextValue() const {
    return toString();
  }
  /** @returns whether the image of this label has zero length */
  bool isTrivial() const;

  /** copy value from ... */
  void operator =(const StoredLabel &other);
  void operator =(const TextValue  &zs);
  void operator =(TextKey zs);
  /** convert integer to text and set this to that image */
  void operator =(int value);

  /** compare values, ignore names */
  bool operator ==(const StoredLabel &other) const;
  bool operator ==(const TextValue  &zs) const;
  bool operator ==(TextKey zs) const;

  using Receiver=sigc::slot<void( TextKey)>;
  /** calls the given slot with this.c_str() as its argument */
  void applyTo(Receiver slotty);
  /** on a change to the value will call applyTo with the given slot */
  sigc::connection onChange(Receiver slotty, bool kickme=false);
  /** a slot that will set the value of this */
  Receiver setter();

  /** set from a double, rendering with @param decimals. if decimals==0 render as integer, if <0 then with that many zeroes. ie -1 is the nearest multuple of 10.  */
  void setFrom(double value, int decimals);
}; // class StoredLabel
