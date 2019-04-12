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

Text StoredLabel::toString() const {
  return Text(node.image());
}

bool StoredLabel::isTrivial() const {
  return isEmpty() || node.image().empty();
}

void StoredLabel::operator =(const StoredLabel&other){
  ONNULLREF(other,);//#gtk gave null references from gui editor
  node.setImage(other.node.image());
}

void StoredLabel::operator =(const TextValue &zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const TextValue &zs) const {
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

void StoredLabel::applyTo(sigc::slot<void, TextKey> slotty){
  slotty(c_str());
}

sigc::connection StoredLabel::onChange(sigc::slot<void, TextKey> slotty, bool kickme){
  return node.addChangeWatcher(bind(MyHandler(StoredLabel::applyTo), slotty),kickme);
}

sigc::slot<void, TextKey> StoredLabel::setter(){
  return bind(mem_fun(node, &Storable::setImageFrom), Storable::Edited);
}

void StoredLabel::setFrom(double value, int decimals){
  auto nf=NumberFormatter(decimals);
  node.setImage(nf.format(value));
}
