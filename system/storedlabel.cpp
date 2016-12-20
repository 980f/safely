#include "storedlabel.h"


StoredLabel::StoredLabel(Storable&node, const TextValue &fallback) : Stored(node){
  if(node.setType(Storable::Textual)) {
    if(node.is(Storable::Parsed)) {
      dbg("Attaching StoredLabel to non-textual Storable, node %s", node.bugName);
    }
  }
  setDefault(fallback);
}

void StoredLabel::setDefault(const TextValue &deftext){
  node.setDefault(deftext);
}

const char *StoredLabel::c_str() const {
  return node.image().c_str();
}

TextValue StoredLabel::toString() const {
  return node.image();
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

void StoredLabel::applyTo(sigc::slot<void, const char *> slotty){
  slotty(c_str());
}

sigc::connection StoredLabel::onChange(sigc::slot<void, const char *> slotty){
  return node.addChangeWatcher(bind(MyHandler(StoredLabel::applyTo), slotty));
}

sigc::slot<void, const char *> StoredLabel::setter(){
  return bind(mem_fun(node, &Storable::setImageFrom), Storable::Edited);
}
