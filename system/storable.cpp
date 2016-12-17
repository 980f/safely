#include "logger.h"
#include "storable.h"
//#include "uuid.h"

int Storable::instances(0);

using namespace sigc;
//using namespace Glib;

//the following are only usable within Storable
#define ForKidsConstly(list) for(ConstChainScanner<Storable> list(wad); list.hasNext(); )
#define ForKids(list) for(ChainScanner<Storable> list(wad); list.hasNext(); )

//legacy stuff, replace one or the other
#define ForWad ForKids(list)
#define ForWadConstly ForKidsConstly(list)


Storable::Storable(NodeName name, bool isVolatile) : isVolatile(isVolatile), type(NotKnown), q(Empty), number(0), parent(nullptr), strictlyOrdered(false), sorted(false), index(-1), enumerated(nullptr), name(name){
  ++instances;
  if(isVolatile) {
    dbg("creating volatile node %s", fullName().c_str());
  }
}

Storable::Storable(bool isVolatile) : isVolatile(isVolatile), type(NotKnown), q(Empty), number(0), parent(nullptr), strictlyOrdered(false), sorted(false), index(-1), enumerated(nullptr), name(""){
  ++instances;
  if(isVolatile) {
    dbg("creating volatile node %s", fullName().c_str());
  }
}

Storable::~Storable(){
  --instances;
  if(instances<0) {
    wtf("freed more storables than we created!");//which can happen if we double free.
  }
  wad.clear(); //while this is automatic I sometimes want to watch it happen just for this class.
  //remove all subnodes but send no signals.
}

void Storable::notify() const {
  if(++recursionCounter>1) {
    wtf("recursing %d in %s",recursionCounter,fullName().c_str());
  }
  watchers.send();
  recursiveNotify();
  --recursionCounter;
}

void Storable::recursiveNotify() const {
  if(isVolatile) {
    return;
  }
  if(parent) {
    parent->childwatchers.send();
    parent->recursiveNotify();
  }
}

Storable &Storable::precreate(NodeName name){
  setType(Wad); //if we are adding a child we must be a wad.
  if(q == Empty) {
    q = Defaulted;
  }
  also(!isVolatile); //we altered the number of entities contained
  Storable &noob(*new Storable(name, false)); //only parent needs volatile flag as that stops recursive looking at the children.
  noob.parent = this;
  //  //the following trick was devised when 'parent' was dropped,
  //  noob.addChangeMother(childwatchers.propagator());//any change to children propagates to parent's childwatchers
  //  noob.addChangeWatcher(childwatchers.propagator());//any change to self triggers parent's childwatchers
  //  //end of trick.
  return noob;
} // precreate

void Storable::setName(NodeName name){
  this->name = name;
}

void *Storable::raw(){
  return static_cast<void *>(this);
}

bool Storable::setType(Type newtype){
  if(isTrivial()) { //don't spew debug info if normal creation.
    type = newtype;
    return false; //changing the type of an empty item is not worthy of note.
  }
  if(changed(type, newtype)) { //actually changed a non-trivial node
    // dbg("changed type for %s", fullName().c_str());
    return true;
  }
  return false;
} // setType

Storable::Type Storable::getType() const {
  return type;
}

bool Storable::setQuality(Quality quality){
  if(q == Empty) {
    return changed(q, quality);
  }
  q = quality;
  return false;
}

void Storable::setEnumerizer(const Enumerated *enumerated){
  if(changed(Storable::enumerated, enumerated)) {
    if(enumerated) {
      if(q >= Parsed && is(Storable::Textual)) { //if already has a text value
        number = enumerated->valueOf(text.c_str()); //reconcile numeric value
      } else {
        if(type == NotKnown) {
          setType(Storable::Textual); //todo:1 probably should be numeric and numeric should check for presence of enumerated, or add a specific Storable::Enumerated to
                                      // reduce redundant checks.
        }
        text = enumerated->token(number);
      }
    } else {
      //todo:1 should we do anything when the text is removed?
      setType(Storable::Numerical); //so booleans which were labeled solely for a gui are saved as canonical false/true
    }
  }
} // setEnumerizer

const Enumerated *Storable::getEnumerizer() const {
  return enumerated;
}

//return whether node was altered
bool Storable::convertToNumber(bool ifPure){
  if(is(Storable::Numerical)) {
    return false;
  } else {
    bool impure(true);
    double ifNumber(toDouble(text.c_str(), &impure));

    if(!ifPure || !impure) {
      setType(Storable::Numerical);
      setNumber(ifNumber, q);
      return true;
    } else {
      return false;
    }
  }
} // convertToNumber

bool Storable::isTrivial() const {
  return type == NotKnown || q == Empty;
}

bool Storable::is(Type ty) const {
  return q != Empty && type == ty;
}

bool Storable::is(Storable::Quality q) const {
  return q == this->q;
}

bool Storable::isModified() const {
  if(isVolatile) {
    return false;
  }
  switch(type) {
  default:
  case NotKnown:
    return false;

  case Wad:
    //investigate all children:
    ForKidsConstly(list){
      if(list.next().isModified()) {
        return true;
      }
    }
  //#join:
  case Numerical:
  case Textual:
    return ChangeMonitored::isModified();
  } // switch
} // isModified

bool Storable::wasModified(){
  bool thiswas = ChangeMonitored::wasModified(); //#reset flag regardless of further considerations

  if(isVolatile) { //# this is the primary purpose of the isVolatile flag, to indicate 'not modified' even if node is.
    return false;
  }
  switch(type) {
  default:
  case NotKnown:
    return false;

  case Wad: { //investigate all children:
    int changes = 0;   //only count node's own changed if no child is changed
    ForKids(list){
      if(list.next().wasModified()) {
        ++changes;
      }
    }
    return changes > 0 || thiswas;
  }
  case Numerical:
  case Textual:
    return thiswas;
  } // switch
} // wasModified

int Storable::listModified(sigc::slot<void, Ustring> textViewer) const {
  if(isVolatile) {
    return 0;
  }
  switch(type) {
  default:
  case NotKnown:
    return 0;

  case Wad: {
    int changes = 0;
    ForKidsConstly(list){
      const Storable &child(list.next());

      changes += child.listModified(textViewer);   //recurse
    }
    if(!changes && ChangeMonitored::isModified()) {   //try not to report propagated changes already reported by loop above.
      textViewer(fullName() + ":reorganized");
      ++changes;
    }
    return changes;
  }
  case Numerical:
  case Textual:
    if(ChangeMonitored::isModified()) {
      textViewer(Ustring::compose("%1:%2", fullName(), image()));
      return 1;
    }
    return 0;
  } // switch
} // listModified

Ustring Storable::fullName() const {
  Ustring path(parent ? parent->fullName() : "root");

  path.append("|");
  path.append(name.empty() ? Ustring::compose("[%1]", ownIndex()) : Ustring(name));
  return path;
}

connection Storable::addChangeWatcher(const SimpleSlot &watcher, bool kickme) const {
  if(kickme) {
    watcher();
  }
  return watchers.connect(watcher);
}

connection Storable::addChangeMother(const SimpleSlot &watcher, bool kickme) const {
  if(kickme) {
    watcher();
  }
  return childwatchers.connect(watcher);
}

void Storable::clone(const Storable &other){ //todo:2 try to not trigger false change indications.
  filicide(); //dump the present wad, we are cloning, not merging
  type = other.type; //# don't use setType(), skip error notice etc since cloning is done to reuse storage.
  q = other.q; //# don't use setQuality()
  setName(other.name);
  enumerated = other.enumerated;
  switch(other.type) {
  default:
  case NotKnown:
    dbg("!Unknown node in tree being copied");
    return; //

  case Numerical:
    number = other.number;
    break;
  case Textual:
    text = other.text;
    break;
  case Wad: //copy preserving order
    for(ConstChainScanner<Storable> list(other.wad); list.hasNext(); ) {
      createChild(list.next());
    }
    break;
  } /* switch */
} // clone

void Storable::assignFrom(const Storable &other){
  if(&other==nullptr) {
    return; //breakpoint, probably a pathological case.
  }
  switch(type) {
  default:
  case NotKnown:
    if(other.is(Numerical)) {
      setNumber(other.number);
    } else if(other.is(Textual)) {
      setImage(other.image());
    }
    break;
  case Numerical:
    setNumber(other.number);
    break;
  case Textual:
    setImage(other.image()); //which can give us the image of a number
    break;
  case Wad:
    //implemented for use case of nodes are the backing store of object of identical Stored class
    //we can't trust node order, we must name-match to handle classes built with different versions of software.
    //we must pull values from other, other may have stale nodes (in intended use).
    ForWad {
      Storable &kid(list.next());//from datum?
      const Storable* older(nonTrivial(kid.name) ? other.existingChild(kid.name) : other.wad[kid.ownIndex()]);
      if(older) {
        kid.assignFrom(*older);
      } else {
        wtf("missing node in assignFrom: this %s, other %s, node %s ",this->fullName().c_str(),other.fullName().c_str(),kid.name.empty() ? "(nameless)" : kid.name.buffer());
      }
    }
    break;
  } /* switch */
} // assignFrom

double Storable::setValue(double value, Storable::Quality quality){
  bool notifeye = changed(number, value);

  notifeye |= setQuality(quality);
  if(enumerated) {
    //if enumerized then leave the type as is and update text
    text = enumerated->token(value);
  } else {
    notifeye |= setType(Numerical);
  }
  also(notifeye); //record changed, but only trigger on fresh change
  if(notifeye) {
    notify();
  }
  return value;
} // setValue

void Storable::setImageFrom(const char *value, Storable::Quality quality){
  bool notifeye = false;

  if(isTrivial()) { //don't notify or detect change, no one is allowed to watch an uninitialized node
    text = value;
    setType(Textual);
    setQuality(quality);
  } else {
    notifeye = changed(text, value);  //todo:00 don't use changed template, do inline to avoid casting
    notifeye |= setQuality(quality);
  }
  notifeye |= setType(Textual);
  also(notifeye); //record changed, but only trigger on fresh change
  if(notifeye) {
    notify();
  }
} // setImageFrom

void Storable::setImage(const Ustring &value, Quality quality){
  setImageFrom(value.c_str(), quality);
}

Ustring Storable::image(void) const {
  switch(type) {
  case Textual:
    return text;

  case Numerical:
    if(enumerated) {
      return enumerated->token(number);
    } else {
      return Ustring::format(/*std::setprecision(12),*/ number);
    }
  case Wad:
    return Ustring::compose("[%1] Value(s)", numChildren());

  default:
    return "(unknown)";
  } // switch
} // image

void Storable::setDefault(const Ustring &value){
  if(q == Empty || q == Defaulted) {
    setImage(value, Defaulted);
  }
}

bool Storable::operator ==(const Ustring &zs){
  return type == Textual && text == zs;
}

ChainScanner<Storable> Storable::kinder(){
  return ChainScanner<Storable>(wad);
}

ConstChainScanner<Storable> Storable::kinder() const {
  return ConstChainScanner<Storable>(wad);
}

Storable *Storable::existingChild(NodeName childName){
  //nameless nodes might be mixed in with named ones:
  ForWad {
    Storable &one(list.next());
    if(one.name.is(childName)) {
      return &one;
    }
  }
  return 0;
}

const Storable *Storable::existingChild(NodeName childName) const {
  //nameless nodes might be mixed in with named ones:
  if(nonTrivial(childName)) {//added guard to make assignFrom( a wad) easier to code.
    ForWadConstly {
      const Storable &one(list.next());
      if(one.name.is(childName)) {
        return &one;
      }
    }
  }
  return nullptr;
} // Storable::existingChild

Storable *Storable::findChild(NodeName path, bool autocreate){
  TextPointer splitbuf(path);//makes a copy, so that we can poke nulls into it.
  CharScanner splitter(splitbuf.buffer(),1 + strlen(path));
  Storable *searcher = this;
  while(searcher) {
    CharScanner split = splitter.split('|'); //#these are internal node names, not user entered text, so we can use any seperator char we wish.
    if(split.hasNext()) {
      if(autocreate) {
        searcher = &searcher->child(split.internalBuffer());//we can use internalBuffer as the splitter and the 1+strlen above together ensure null termination.
      } else {
        searcher = searcher->existingChild(split.internalBuffer());
      }
    } else {
      break;
    }
  }

  return searcher;
} // findChild

/** creates node if not present.*/
Storable &Storable::child(NodeName childName){
  Storable *child = existingChild(childName);
  if(child) {
    return *child;
  }
  return addChild(childName);
}

Storable &Storable::operator ()(NodeName name){
  return child(name);
}

Storable &Storable::operator [](int ordinal){
  if(!has(ordinal)) {
    wtf("nonexisting child of %s referenced by ordinal %d (out of %d).", fullName().c_str(), ordinal, numChildren());
    dumpStack("nth child doesn't exist");
    addChild(); //better than an NPE so deep in the hierarchy that we don't know where it comes from.
    return *wad.last();
  }
  return *wad[ordinal];
}

const Storable &Storable::nth(int ordinal) const {
  if(!has(ordinal)) {
    wtf("nonexisting child referenced by ordinal %d (out of %d).", ordinal, numChildren());
  }
  return *wad[ordinal];
}

int Storable::indexOf(const Storable &node) const {
  return wad.indexOf(&node);
}

Storable &Storable::addChild(NodeName childName){
  Storable &noob(precreate(childName));

  return finishCreatingChild(noob);
}

Storable &Storable::createChild(const Storable &other){
  Storable &noob(precreate(other.name));

  noob.clone(other);
  return finishCreatingChild(noob);
}

Storable &Storable::finishCreatingChild(Storable &noob){
  noob.index = wad.quantity();
  wad.append(&noob); //todo:sorted insert
  /* notify on add caused more problems than it solved. Only StoredGroups should need watchers that care about new entities (for things like a sum). Adding a new member
   * to a struct should not matter to anything.
   * This is consistent with us not notifying on remove.
   * notify();
   */
  return noob;
}

Storable &Storable::addWad(int qty, Storable::Type type, NodeName name){
  Storable &noob(precreate(name));

  noob.presize(qty, type);
  return finishCreatingChild(noob);
}

void Storable::presize(int qty, Storable::Type type){
  int i = qty - numChildren();

  while(i-- > 0) {
    Storable &kid = addChild();
    kid.setType(type);
    //and allow constructed default values to persist
    setQuality(Defaulted); //not using Empty as that often masks the type being set.
  }
}

bool Storable::remove(int which){
  if(has(which)) {
    wad.removeNth(which);
    //renumber children, must follow removal to make for-loop cute
    for(int ci = wad.quantity(); ci-- > which; ) { //from last downto item newly dropped into 'which' slot
      --(wad[ci]->index);
    }
    also(true);
    notify(); //added this notify so that NodeEditor deletes can show on screen.
    return true;
  } else {
    return false;
  }
} // remove

bool Storable::removeChild(Storable &node){
  return remove(indexOf(node));
}

void Storable::filicide(){
  also(numChildren() != 0); //mark for those who poll changes...
  wad.clear(); //remove WITHOUT notification, we only call filicide when we are cloning
}

void Storable::getArgs(ArgSet &args, bool purify){
  ForKids(list){
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
} // getArgs

void Storable::setArgs(ArgSet &args){
  while(args.hasNext()) {
    int which = args.ordinal();
    if(has(which)) {
      wad[which]->setNumber(args.next());
    }
  }
}

/////////////////

Stored::Stored(Storable &node) : duringConstruction(true), node(node), refreshed(true){
  onAnyChange(MyHandler(Stored::doParse), false); //# can't call onParse here as required children might not exist.
  node.preSave.connect(MyHandler(Stored::onPrint));
}

Stored::~Stored(){
  //#nada
}

void Stored::doParse(){
  onParse();
}

//this is a dangerous function, the returned pointer must not be retained-data might be freed later.
const char *Stored::rawText() const {
  return node.text.c_str();
}

sigc::connection Stored::onAnyChange(SimpleSlot slotty, bool kickme){
  return node.is(Storable::Wad) ? node.addChangeMother(slotty, kickme) : node.addChangeWatcher(slotty, kickme);
}

void Stored::markTrivial(){
  node.isVolatile = true;
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

void Stored::getArgs(ArgSet &args){
  node.getArgs(args);
}

void Stored::setArgs(ArgSet &args){
  node.setArgs(args);
}

/** watcher is invoked on first call to watchArgs but not subsequent ones, should move that to caller*/
sigc::connection Stored::watchArgs(const SimpleSlot &watcher, bool kickme){
  return onAnyChange(watcher, kickme);
}

void Stored::allocArgs(int qty){
  node.presize(qty, Storable::Numerical);
}

void Stored::getArgs(NodeName child, ArgSet &args){
  node.child(child).getArgs(args, false);
}

void Stored::setArgs(NodeName child, ArgSet &args){
  Storable &array = node.child(child);

  array.setArgs(args);
}

bool Stored::isEmpty() const {
  return this == 0 || &node == 0 || node.q == Storable::Empty;
}

void Stored::triggerWatchers(){
  node.notify();
}

SimpleSlot Stored::notifier(){
  return sigc::mem_fun(node, &Storable::notify);
}

///////////////////////

//////////////////////

StoredLabel::StoredLabel(Storable &node, const Ustring &fallback) : Stored(node){
  if(node.setType(Storable::Textual)) {
    if(node.is(Storable::Parsed)) {
      dbg("Attaching StoredLabel to non-textual Storable, node %s", node.fullName().c_str());
    }
  }
  setDefault(fallback);
}

void StoredLabel::setDefault(const Ustring &deftext){
  node.setDefault(deftext);
}

const char *StoredLabel::c_str() const {
  return node.image().c_str();
}

Ustring StoredLabel::toString() const {
  return node.image();
}

bool StoredLabel::isTrivial() const {
  return isEmpty() || node.image().empty();
}

void StoredLabel::operator =(const StoredLabel &other){
  if(&other) {
    node.setImage(other.node.image());
  } else {
    wtf("null rhs in StoredLabel operator =");
  }
}

void StoredLabel::operator =(const Ustring &zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const Ustring &zs) const {
  return node.image() == zs;
}

void StoredLabel::operator =(const char *zs){
  node.setImage(zs);
}

bool StoredLabel::operator ==(const char *zs) const {
  return node.image() == zs;
}

bool StoredLabel::operator ==(const StoredLabel &other) const {
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

///////////////////////
StoredListReuser::StoredListReuser(Storable &node, int wadding) : node(node), wadding(wadding), pointer(0){
  //#nada
}

Storable &StoredListReuser::next(){
  if(node.has(pointer)) {
    if(wadding) {
      node[pointer].presize(wadding);//wadding is minimum size of nodes to be created.
    }
    return node[pointer++]; //expedite frequent case
  }
  ++pointer;
  if(wadding) {
    return node.addWad(wadding);
  } else {
    return node.addChild();
  }
} // next

int StoredListReuser::done(){
  //pointer is quantity that have been done
  //killer is init to number that exist
  //if they are equal then all is well.
  for(int killer = node.numChildren(); killer-- > pointer; ) { //#efficient order, no shuffling of ones that will then also be whacked.
    node.remove(killer);
  }
  return pointer;
}

//////////////////////
Storable::Freezer::Freezer(Storable &node, bool childrenToo, bool onlyChildren) : childrenToo(childrenToo), onlyChildren(onlyChildren), node(node){
  freezeNode(node);
}

Storable::Freezer::~Freezer(){
  if(!onlyChildren) {
    node.watchers.ungate();
  }
  if(childrenToo) {
    node.childwatchers.ungate();
  }
}

void Storable::Freezer::freezeNode(Storable &node, bool childrenToo, bool onlyChildren){
  if(!onlyChildren) {
    node.watchers.gate();
  }
  if(childrenToo) {
    node.childwatchers.gate();
  }
}

////////////////////////

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

void Stored::setName(NodeName name){
  node.setName(name);
}
