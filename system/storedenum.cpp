#include "storedenum.h"


StoredEnum::StoredEnum(Storable &node, const Enumerated &enumerated, int def) :
  Stored(node){
  node.setEnumerizer(&enumerated);//has a side efect of converting stored text to number, but leaves node marked 'textual'
  node.setDefault(def);
}

StoredEnum::operator int() const {
  return node.getNumber< int >();
}

int StoredEnum::native() const {
  return node.getNumber< int >();
}

int StoredEnum::setto(int newnum){
  return node.setNumber(newnum);
}

Ustring StoredEnum::toString(){
  return node.image();
}

SimpleSlot StoredEnum::setLater(int value){
  return sigc::hide_return(sigc::bind(sigc::mem_fun< int, int >(this, &StoredEnum::operator =), value));
}

sigc::slot<void, int> StoredEnum::setter(){
  return sigc::hide_return( MyHandler(StoredEnum::setto));
}

sigc::slot<int> StoredEnum::getLater(){
  return MyHandler(StoredEnum::native);
}

SimpleSlot StoredEnum::applyTo(sigc::slot<void, int> functor){
  return sigc::compose(functor,getLater());
}

sigc::connection StoredEnum::sendChanges(sigc::slot<void, int> functor, bool kickme){
  return onAnyChange(applyTo(functor),kickme);
}

void StoredEnum::reEnumerate(const Enumerated &enumerated){
  node.setEnumerizer(&enumerated);//has a side efect of converting stored text to number, but leaves node marked 'textual'
}
