#ifndef STOREDRANGE_H
#define STOREDRANGE_H
#include "stubstorable.h"
//#include "storednumeric.h"
#include "ranged.h"

#include "stddef.h"

class StoredRange : public Stored {
public://made public for editor access
  StoredReal min;
  StoredReal max;
public:
  ConnectClass(StoredRange);
  StoredRange(Storable &node, double maximum, double minimum = 0);
  StoredRange(double maximum, double minimum = 0);
  virtual ~StoredRange();
  /** similar to Ranged */
  void setto(double higher, double lower);
  /** formerly a cached member, but update timing was too hard to deal with.*/
  Ranged ranged()const;//
  /** @returns a slot that when invoked returns whether the range is nonTrivial */
//  sigc::slot<bool> rangeChecker()const;
  /** @returns whether the range has a non-zero width and is normally ordered.
   it is useful for slots versus accessing raw as raw itself is non sigc-trackable.*/
  bool isUseful()const;
};

#endif // STOREDRANGE_H
