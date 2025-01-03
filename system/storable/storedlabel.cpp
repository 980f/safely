#include "storedlabel.h"


StoredLabel::StoredLabel(Storable&node, const char *fallback) : Stored(node){
  if(node.setType(Storable::Textual)) {
    if(node.is(Storable::Parsed)) {
      dbg("Attaching StoredLabel to non-textual Storable, node %s", node.fullName().c_str());
    }
  }
  setDefault(fallback);
}

void StoredLabel::setDefault(const char *deftext){
  node.setDefault(deftext);
}

TextKey StoredLabel::c_str() const {
  return node.image();
}

Text StoredLabel::toString() const {
  return Text(node.image());
}

bool StoredLabel::isTrivial() const {
  return isEmpty() || node.image().empty();
}

void StoredLabel::operator =(const StoredLabel&other){
  if(&other) {//#yes, we did get null references from gui editor
    node.setImage(other.node.image());
  } else {
    wtf("null rhs in StoredLabel operator =");
  }
}

void StoredLabel::operator =(const Text &zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const Text &zs) const {
  return node.image() == zs;
}

void StoredLabel::operator =(int value){
  Text image=NumberFormatter::makeNumber(value);
  node.setImage(image.c_str());
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

void StoredLabel::applyTo(Receiver slotty){
  slotty(c_str());
}

sigc::connection StoredLabel::onChange(Receiver slotty, bool kickme){
  return node.addChangeWatcher(bind(MyHandler(StoredLabel::applyTo), slotty),kickme);
}

StoredLabel::Receiver StoredLabel::setter(){
  return bind(mem_fun(node, &Storable::setImageFrom), Storable::Edited);
}

void StoredLabel::setFrom(double value, int decimals){
  auto nf=NumberFormatter(decimals);
  node.setImage(nf.format(value));
}
