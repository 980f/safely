#include "storedlabel.h"


StoredLabel::StoredLabel(Storable&node, const TextValue &fallback) : Stored(node){
  if(node.setType(Storable::Textual)) {
    if(node.is(Storable::Parsed)) {
      dbg("Attaching StoredLabel to non-textual Storable, node %s", node.fullName().c_str());
    }
  }
  setDefault(fallback);
}

void StoredLabel::setDefault(const TextValue &deftext){
  node.setDefault(deftext.c_str());
}

TextKey StoredLabel::c_str() const {
  return node.image();
}

TextValue StoredLabel::toString() const {
  return TextValue(node.image());
}

bool StoredLabel::isTrivial() const {
  return isEmpty() || node.image().empty();
}

void StoredLabel::operator =(const StoredLabel&other){
  if(&other) {
    node.setImage(other.node.image());
  } else {
    wtf("null rhs in StoredLabel operator =");
  }
}

void StoredLabel::operator =(const TextValue &zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const TextValue &zs) const {
  return node.image() == zs;
}

void StoredLabel::operator =(const char *zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const char *zs) const {
  return node.image() == zs;
}

bool StoredLabel::operator ==(const StoredLabel&other) const {
  return node.image() == other.node.image();
}

void StoredLabel::applyTo(sigc::slot<void, TextKey> slotty){
  slotty(c_str());
}

sigc::connection StoredLabel::onChange(sigc::slot<void, TextKey> slotty){
  return node.addChangeWatcher(bind(MyHandler(StoredLabel::applyTo), slotty));
}

sigc::slot<void, TextKey> StoredLabel::setter(){
  return bind(mem_fun(node, &Storable::setImageFrom), Storable::Edited);
}
