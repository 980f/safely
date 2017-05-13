#include "stored.h"

/////////////////

Storable &Stored::Groot(TextKey pathname){  
  if(Cstr(pathname).empty()){
    return groot;
  }
  Storable *node=Stored::groot.findChild(pathname,true);
  if(node){
    return *node;
  }
  //else a relative path that looked back past groot (or an independent tree's root)
  return groot.child(pathname);//which most likely will be non-functional, but at least not null.
}

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
int Stored::parentIndex(int generations) const {
  Storable *parent = &(node);

  while(generations-- > 0) {
    if(!parent) {
      return -1;
    }
    parent = parent->parent;
  }
  return parent ? parent->ownIndex() : -1;
} // parentIndex

int Stored::index() const {
  return node.ownIndex();
}

bool Stored::indexIs(int which) const {
  return index() == which;
}

sigc::slot<int> Stored::liveindex() const {
  //  return MyHandler(Stored::index);//obvious
  return mem_fun(node, &Storable::ownIndex); //faster
}

void Stored::getArgs(ArgSet&args){
  node.getArgs(args);
}

void Stored::setArgs(ArgSet&args){
  node.setArgs(args);
}

/** watcher is invoked on first call to watchArgs but not subsequent ones, should move that to caller*/
sigc::connection Stored::watchArgs(const SimpleSlot&watcher, bool kickme){
  return onAnyChange(watcher, kickme);
}

void Stored::allocArgs(int qty){
  node.presize(qty, Storable::Numerical);
}

void Stored::getArgs(NodeName child, ArgSet&args){
  node.child(child).getArgs(args, false);
}

void Stored::setArgs(NodeName child, ArgSet&args){
  Storable&array = node.child(child);

  array.setArgs(args);
}

bool Stored::isEmpty() const {
  //first two terms avert NPE's on horribly bad code, should never fire.
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

NodeName Stored::getName() const {
  return node.name;
}

Storable Stored::groot("/",true);
