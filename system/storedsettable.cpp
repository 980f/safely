#include "storedsettable.h"

//using namespace Glib;


SettableAdaptor::SettableAdaptor(Storable &node,Settable&unstored) :
  Stored(node),
  wrapped(unstored){
  allocArgs(wrapped.numParams());
  duringConstruction = false;
  onParse();
}

SettableAdaptor::SettableAdaptor(Storable &node, NodeName childname, Settable &unstored) :
  Stored(node.child(childname)),
  wrapped(unstored){
  allocArgs(wrapped.numParams());
  duringConstruction = false;
  onParse();
}

void SettableAdaptor::onParse(){
  if(duringConstruction) {
    return;//else we get called n-times
  }
  makeArgs(wrapped.numParams());
  getArgs(args);
  args.rewind();
  wrapped.setParams(args);
}

void SettableAdaptor::onPrint(){
  makeArgs(wrapped.numParams());
  wrapped.getParams(args);
  args.rewind();
  setArgs(args);
}
