#ifndef STOREDIPV4ADDRESS_H
#define STOREDIPV4ADDRESS_H

#include "storable.h"
#include "storednumeric.h"
#include "storedlabel.h"

#include "textpointer.h"

/** stored IP address.
 *  initially only supports dotted decimal, does not locally do DNS
 */

class StoredIPV4Address : public Stored {
  void makeNumber();
  void makeText();
public:
  StoredIPV4Address(Storable &node);
  /** the 32 bit address, in host order */
  StoredNumeric<u32> resolved;
  /** the dotted decimal representation of this address/*/
  StoredLabel dotted;
  /** @returns dotted decimal text given resolved address */
  static Text dotstring(u32 ipv4);
}; // class StoredIPV4Address

#endif // STOREDIPV4ADDRESS_H
