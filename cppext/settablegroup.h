#ifndef SETTABLEGROUP_H
#define SETTABLEGROUP_H

#include "settable.h"


/** eacn setting gets its own tag */
template <class SG1> struct SettingTag {
  /** unique id for the settable */
  char id;
  /** item which reads and writes ArgSets */
  Settable SG1::*settable;
  /** another ArgSet reader and writer, which gets the tail end of any argset that the primary has had its way with */
  Settable SG1::*sibling=0;
};

/** for what was once HasSettings we make a table */


/** dispatch mechanism for manageing a group of Settable*/
class SettableGroup
{
public:
  SettableGroup();
};

#endif // SETTABLEGROUP_H
