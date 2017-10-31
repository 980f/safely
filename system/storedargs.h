#ifndef STOREDARGS_H
#define STOREDARGS_H

#include "argset.h"
#include "stored.h"

/** wraps a node that contains the args of an argset, see HasSettings */
class StoredArgs : public Stored {
public:
  StoredArgs(Storable &node);
  void allocArgs(unsigned qty);

  sigc::connection watchArgs(const SimpleSlot &watcher, bool kickme = false);

  void getArgs(ArgSet &args, bool purify=true);
  void setArgs(ArgSet &args);
  /** user wants children*/
  void getArgs(TextKey child, ArgSet &args, bool purify=true);
  void setArgs(TextKey child, ArgSet &args);

public://formerly part of storable
  static void getArgsFrom(Storable &node, ArgSet &args, bool purify=true);
  static void setArgsOn(Storable &node,ArgSet &args);
}; // class StoredArgs

#endif // STOREDARGS_H
