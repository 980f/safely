#include "storedlabel.h"

#include "numbertextifier.h"

StoredText::StoredText(Storable&node, const TextValue &fallback) : Stored(node){
  if(node.setType(Storable::Textual)) {
    if(node.is(Storable::Parsed)) {
      dbg("Attaching StoredText to non-textual Storable, node %s", node.fullName().c_str());
    }
  }
  setDefault(fallback);
}

void StoredText::setDefault(const TextValue &deftext){
  node.setDefault(deftext.c_str());
}

TextKey StoredText::c_str() const {
  return node.image();
}

Text StoredText::toString() const {
  return Text(node.image());
}

bool StoredText::isTrivial() const {
  return isEmpty() || node.image().empty();
}

void StoredText::operator =(const StoredText&other){
  if(&other) {//yes, we can get null references.
    node.setImage(other.node.image());
  } else {
    wtf("null rhs in StoredText operator =");
  }
}

void StoredText::operator =(const TextValue &zs){
  node.setImage(zs);
}

bool StoredText::operator ==(const TextValue &zs) const {
  return node.image() == zs;
}

void StoredText::operator =(int value){
  Text image=NumberTextifier::makeNumber(value);
  node.setImage(image.c_str());
}

void StoredText::operator =(const char *zs){
  node.setImage(zs);
}

bool StoredText::operator ==(const char *zs) const {
  return node.image() == zs;
}

bool StoredText::operator ==(const StoredText&other) const {
  return node.image() == other.node.image();
}

void StoredText::applyTo(sigc::slot<void, TextKey> slotty){
  slotty(c_str());
}

sigc::connection StoredText::onChange(sigc::slot<void, TextKey> slotty){
  return node.addChangeWatcher(bind(MyHandler(StoredText::applyTo), slotty));
}

sigc::slot<void, TextKey> StoredText::setter(){
  return bind(mem_fun(node, &Storable::setImageFrom), Storable::Edited);
}
