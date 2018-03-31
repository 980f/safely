#include "storedargs.h"

StoredArgs::StoredArgs(Storable &node) : Stored(node){

}

void StoredArgs::getArgs(ArgSet&args,bool purify){
  getArgsFrom(node,args,purify);
}

void StoredArgs::setArgs(ArgSet&args){
  setArgsOn(node,args);
}

void StoredArgs::allocArgs(unsigned qty){
  node.presize(qty, Storable::Numerical);
}

sigc::connection StoredArgs::watchArgs(const SimpleSlot&watcher, bool kickme){
  return onAnyChange(watcher, kickme);
}

void StoredArgs::getArgs(TextKey child, ArgSet&args, bool purify){
  getArgsFrom(node.child(child),args, purify);
}

void StoredArgs::setArgs(TextKey child, ArgSet&args){
  setArgsOn(node.child(child),args);
}

void StoredArgs::getArgsFrom(Storable &node,ArgSet&args,bool purify){
  for(auto list = node.kinder(); list.hasNext(); ) {
    if(!args.hasNext()) {
      break;
    }
    args.next() = list.next().getNumber<double>();
  }
  if(purify) {
    while(args.hasNext()) {
      args.next() = 0.0;
    }
  }
}

void StoredArgs::setArgsOn(Storable &node, ArgSet &args){
  while(args.hasNext()) {
    unsigned which = args.ordinal();
    if(node.has(which)) {
      node.nth(which).setNumber(args.next());
    }
  }
}
