#include "storedenum.h"


StoredEnum::StoredEnum(Storable &node, const Enumerated &enumerated, unsigned def) :
  Stored(node){
  node.setEnumerizer(&enumerated);//has a side efect of converting stored text to number, but leaves node marked 'textual'
  node.setDefault(def);
}

StoredEnum::operator unsigned() const {
  return node.getNumber< unsigned >();
}

unsigned StoredEnum::native() const {
  return node.getNumber< unsigned >();
}

unsigned StoredEnum::setto(unsigned newnum){
  return node.setNumber(newnum);
}

Cstr StoredEnum::toString(){
  return node.image();
}

SimpleSlot StoredEnum::setLater(unsigned value){
  return sigc::hide_return(sigc::bind(sigc::mem_fun< unsigned, unsigned >(this, &StoredEnum::operator =), value));
}

sigc::slot<void, unsigned> StoredEnum::setter(){
  return sigc::hide_return( MyHandler(StoredEnum::setto));
}

sigc::slot<unsigned> StoredEnum::getLater(){
  return MyHandler(StoredEnum::native);
}

SimpleSlot StoredEnum::applyTo(sigc::slot<void, unsigned> functor){
  return sigc::compose(functor,getLater());
}

sigc::connection StoredEnum::sendChanges(sigc::slot<void, unsigned> functor, bool kickme){
  return onAnyChange(applyTo(functor),kickme);
}

//void StoredEnum::reEnumerate(const Enumerated &enumerated){
//  node.setEnumerizer(&enumerated);//has a side efect of converting stored text to number, but leaves node marked 'textual'
//}
