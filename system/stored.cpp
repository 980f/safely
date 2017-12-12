#include "stored.h"  //(C) 2017 Andrew L. Heilveil

Stored::Stored(Storable&node) : duringConstruction(true), node(node), refreshed(true){
  //onAnyChange(MyHandler(Stored::doParse), false); //# can't call onParse here as required children might not exist.
//  node.preSave.connect(MyHandler(Stored::onPrint));
}

Stored::~Stored(){
  //#nada
}

void Stored::doParse(){
  onParse();
}

//this is a dangerous function, the returned pointer must not be retained-data might be freed later.
TextKey Stored::rawText() const {
  return node.text.c_str();
}

sigc::connection Stored::onAnyChange(SimpleSlot slotty, bool kickme){
  return node.is(Storable::Wad) ? node.addChangeMother(slotty, kickme) : node.addChangeWatcher(slotty, kickme);
}

void Stored::markTrivial(){
  node.isVolatile = true;
}

void Stored::legacy(TextKey oldname, TextKey newname, bool purgeOld){//purgeOld default is historical
  if(Storable * legacy = node.existingChild(oldname)) {
    node.child(newname).assignFrom(*legacy);
    if(purgeOld) {
      node.remove(legacy->index);
    }
  }
}

/** parent (0) is self, return own index ,if a member of a StoredGroup then this is index within group
 *  parent (1) is node containing the node of interest*/
unsigned Stored::parentIndex(int generations) const {
  return node.parentIndex(generations);
}

unsigned Stored::index() const {
  return node.ownIndex();
}

bool Stored::indexIs(unsigned which) const {
  return index() == which;
}

sigc::slot<unsigned> Stored::liveindex() const {
  //  return MyHandler(Stored::index);//obvious
  return mem_fun(node, &Storable::ownIndex); //faster
}

bool Stored::isEmpty() const {
  //first two terms avert NPE's on horribly bad code, should never fire. (they were added due to gtk using void *'s and occasionally a null leaking into those).
  return this == nullptr || &node == nullptr || node.q == Storable::Empty;
}

void Stored::triggerWatchers(){
  node.notify();
}

SimpleSlot Stored::notifier(){
  return sigc::mem_fun(node, &Storable::notify);
}

void Stored::prepRefresh(){
  refreshed = false;
}

void Stored::isRefreshed(){
  refreshed = true;
}

bool Stored::notRefreshed() const {
  return refreshed == false;
}

TextKey Stored::getName() const {
  return node.name;
}
