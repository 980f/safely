#ifndef STOREDSETTABLE_H
#define STOREDSETTABLE_H

#include "storable.h"
#include "settable.h"
#include "hassettings.h"

template<class SettableClass> class StoredSettable : public Stored  {
public:
  SettableClass object;

  StoredSettable(Storable &node) : Stored(node),object(){
    allocArgs( object.numParams());
    duringConstruction = false;
    onParse();
  }

  void operator = (const SettableClass &other){
    object = other;
  }

  /** add to make sigc::bind more tractable */
  void setto(const SettableClass &other){
    object = other;
  }

  /** first used for manipulating com strings from hardware interface */
  bool parse(CharFormatter &p){
    makeArgs( object.numParams());
    bool ok = HasSettings::parseArgstring(args,p);
    args.rewind();
    object.setParams(args);//write to object
    onPrint(); //in case something like the gui is watching nodes.
    return ok;
  }

  void onParse(){
    if(duringConstruction) {
      return; //else we get called n-times
    }
    makeArgs( object.numParams());
    getArgs(args);//reads from nodes
    args.rewind();
    object.setParams(args);//write to object
  }

  /** hook for actions to perform when node is written to disk.
   *  @returns success, if false then the write must be reported as partially failed.*/
  void onPrint(){
    makeArgs(object.numParams());
    object.getParams(args);//read from object
    args.rewind();
    setArgs(args);//write on nodes.
  }

}; // class StoredSettable

/** useful for embedding Settable things in Stored classes.
 *  The wrapped item is filled from Storage at the time this SettableAdaptor is constructed,
 *  but its value only get put into the Stored data when onPrint is called.
 *  The only automatic onPrint invocation is on treeFile storage.
 *  An editor must therefore invoke onPrint, and changes made while the editor is showing will be reset when on editor close the editor calls onParse.
 */
class SettableAdaptor : public Stored {
public:
  Settable&wrapped;
  SettableAdaptor(Storable &node,Settable&unstored);
  SettableAdaptor(Storable &node, NodeName childname, Settable &unstored);

  void onPrint();
  void onParse();
};


#endif // STOREDSETTABLE_H
