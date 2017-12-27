#include "logger.h"

#include "textkey.h" //toDouble
#include "storable.h"
#include "charformatter.h"

#include "segmentedname.h" //for debug reports
#include "dottedname.h"

//this is not a class member so that we don't force pathparser.h on all users:
static const PathParser::Rules slasher('/',false,true);// '.' gives java property naming, '/' would allow use of filename classes. '|' was used for gtkwrappers access
static const char PathSep = '/';
/** global/shared root, the 'slash' node for findChild */
__attribute__ ((init_priority (200)))
Storable Storable::Slash("/");
/** access for JsonSocket */
Storable::Mirror *Storable::remote = nullptr;

Storable &Storable::Groot(TextKey pathname){
  if(Cstr(pathname).empty()) {
    return Slash;
  }
  Storable *node = Slash.findChild(pathname,true);
  if(node) {
    return *node;
  } else {
    //else a relative path that looked back past groot (or an independent tree's root)
    return Slash.child(pathname);//which most likely will be non-functional, but at least not null.
  }
} // Storable::Groot

bool Storable::Delete(TextKey pathname){
  if(Cstr(pathname).empty()) {
    return false;
  }
  Storable *node = FindChild(pathname,false);
  if(node) {
    if(node->parent) {
      node->parent->removeChild(*node);
      return true;
    } else {
      //root or floating node, can't delete those. Should also not create them! There is no need for floating nodes except as local temps.
      return false;
    }
  } else {
    return false;
  }
} // Storable::Delete

Storable *Storable::FindChild(TextKey pathname, bool autocreate){
  return Slash.findChild(pathname,autocreate);
} // Storable::Delete

////////////

using namespace sigc;

//the following are only usable within Storable
#define ForKidsConstly(list) for(ConstChainScanner<Storable> list(wad); list.hasNext(); )
#define ForKids(list) for(ChainScanner<Storable> list(wad); list.hasNext(); )

/** @returns either a number  or the key value BadIndex (way larger than we allow a wad to be) */
static unsigned numericalName(TextKey name){
  if(name[0]=='#') {
    bool impure(true);
    unsigned which = toDouble(&name[1], &impure);
    if(impure) {
      return BadIndex;
    }
    return which;
  }
  return BadIndex;
} // numericalName

Storable::Storable(TextKey name, bool isVolatile) :
  isVolatile(isVolatile),
  type(NotDefined),
  q(Empty),
  number(),
  parent(nullptr),
  index(BadIndex),
  enumerated(nullptr),
  name(name){
  if(isVolatile) {
    dbg("creating volatile node %s", fullName().c_str());
  }
}

//requires gcc >=4.7
Storable::Storable(bool isVolatile) : Storable("", isVolatile){
}

Storable::~Storable(){
//  --instances;
//  if(!Index(instances).isValid ()) {
//    wtf("freed more storables than we created!"); //which can happen if we double free.
//  }
}

void Storable::notify() const {
  static int recursionCounter = 0;//4debug of notify.
  if(remote) {
    remote->alter(*this);
  }
  if(++recursionCounter>1) {
    wtf("recursing %d in %s",recursionCounter,fullName().c_str());
  }
  //todo:Omni send change
  //#we don't check isvolatile here as volatile nodes are often of singular interest, they just aren't of general interest (should not trigger group watch).
  watchers.send();
  recursiveNotify();
  --recursionCounter;
} // Storable::notify

void Storable::recursiveNotify() const {
  if(isVolatile) {//this check is one of the main reasons for existence of isVolatile, to indicate gratuitous or redundant nodes
    return;
  }
  if(parent) {
    parent->childwatchers.send();
    parent->recursiveNotify();
  }
}

Storable &Storable::precreate(TextKey name){
  setType(Wad); //if we are adding a child we must be a wad.
  if(q == Empty) {
    q = Defaulted;
  }
  also(!isVolatile); //we altered the number of entities contained
  Storable&noob(*new Storable(name, false)); //only parent needs volatile flag as that stops recursive looking at the children.
  noob.parent = this;
  return noob;
}

void Storable::Rename(TextKey newname){
#if 0
  unsigned last = numChildren();
  Storable &noob = createChild(*this,newname);
  wad.swap(noob.ownIndex(),this->ownIndex());
  noob.index = ownIndex();
  this->index = last;//in case remover looks at it.
  parent->remove(last);//which is this so we should skedaddle
#else
  const_cast<Text&>(name) = newname;
#endif
} // Storable::Rename

void *Storable::raw(){
  return static_cast<void *>(this);
}

bool Storable::setType(Type newtype){
  if(isTrivial()) { //don't spew debug info if normal creation.
    type = newtype;
    return false; //changing the type of an empty item is not worthy of note.
  }
  if(changed(type, newtype)) { //actually changed a non-trivial node
    // dbg("changed type for %s", );
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
  if(changed(this->enumerated, enumerated)) {
    if(enumerated) {
      number.changeInto(NumericalValue::Counting);//enumness is more important than textuality.
      if((q >= Parsed) && is(Storable::Textual)) { //if already has a text value
        number = enumerated->valueOf(text.c_str()); //reconcile numeric value
      } else {
        if(type <= Uncertain) {
          setType(Storable::Textual); //todo:0 probably should be numeric and numeric should check for presence of enumerated, or add a specific Storable::Enumerated to
                                      // reduce redundant checks.
        }
        text = enumerated->token(number);
      }
    } else {
      number.changeInto(NumericalValue::Counting);//symbol-free enum, probably a programmer's error.
      //todo:1 should we do anything when the text is removed?
      setType(Storable::Numerical); //so booleans which were labeled solely for a gui are saved as canonical false/true
    }
  }
} // setEnumerizer

const Enumerated *Storable::getEnumerizer() const {
  return enumerated;
}

//return whether node was altered
bool Storable::convertToNumber(NumericalValue::Detail subtype){
  setType(Storable::Numerical);
  return number.changeInto(subtype);
} // Storable::convertToNumber

bool Storable::resolve(bool recursively){
  if(is(Storable::Uncertain)) {

    if(false ) {//todo: if needed do a chr scan for numerical appearance.
      return true;
    } else {//it must be text
      setType(Storable::Textual);
      return true;
    }
  }
  if(recursively && is(Wad)) {
    ForKidsConstly(list){
      list.next().resolve(true);
    }
  }
  return false;
} // convertToNumber

bool Storable::isTrivial() const {
  return type == NotDefined || q == Empty;
}

bool Storable::is(Type ty) const {
  return q != Empty && type == ty;
}

bool Storable::is(Storable::Quality q) const {
  return q == this->q;
}

bool Storable::isModified() const {
  if(isVolatile) {//because we use this primarily to determine if the container needs to be saved, we leave out nodes that don't get saved.
    return false;
  }
  switch(type) {
  case Wad:
    //investigate all children:
    ForKidsConstly(list){
      if(list.next().isModified()) {
        return true;
      }
    }
  //#JOIN
  case Numerical:
  //#JOIN
  case Textual:
    return ChangeMonitored::isModified();
  default:
  case NotDefined:
    return false;
  } // switch

} // isModified

bool Storable::wasModified(){
  bool thiswas = ChangeMonitored::wasModified(); //#reset flag regardless of further considerations

  if(isVolatile) { //# this is the primary purpose of the isVolatile flag, to indicate 'not modified' even if node is.
    return false;
  }
  switch(type) {
  default:
  case Uncertain:
  case NotDefined:
    return false;

  case Wad: { //investigate all children:  //need bracing to keep 'changes' local.
    unsigned changes = 0;   //only count node's own changed if no child is changed
    ForKids(list){
      if(list.next().wasModified()) {
        ++changes;
        //#don't exit early, we want to run wasModified() on all entities to clear their dirty bits.
      }
    }
    return changes > 0 || thiswas;
  }
  //#JOIN;
  case Numerical:
  //#JOIN;
  case Textual:
    return thiswas;
  } // switch
} // wasModified

Text Storable::fullName() const {
  //non-recursive,
  SegmentedName pathname;
  const Storable *scan = this;

  do {
    pathname.prefix(scan->name);
  } while((scan = scan->parent));

  return PathParser::pack(pathname,slasher);
} // Storable::fullName

unsigned Storable::parentIndex(int generations) const {
  const Storable *genealogist = this;
  while(genealogist&&generations-- > 0) {
    genealogist = genealogist->parent;
  }
  if(genealogist) {
    return genealogist->ownIndex();
  } else {
    return BadIndex;
  }
} // Storable::fullName

connection Storable::addChangeWatcher(const SimpleSlot&watcher, bool kickme) const {
  if(kickme) {
    watcher();
  }
  return watchers.connect(watcher);
}

connection Storable::addChangeMother(const SimpleSlot&watcher, bool kickme) const {
  if(kickme) {
    watcher();
  }
  return childwatchers.connect(watcher);
}

//this is a piece of copy constructor.
void Storable::clone(const Storable&other){ //todo:2 try to not trigger false change indications.
  filicide(); //dump the present wad, we are cloning, not merging
  type = other.type; //# don't use setType()
  q = other.q; //# don't use setQuality()
  //  setName(other.name);
  enumerated = other.enumerated;
  number = other.number;
  text.copy(other.text);//want independent copy
  switch(other.type) {
  //trust compiler to bitch if case missing:--  default:
  case NotDefined:
    dbg("!Unknown node in tree being copied");
    return; //
  case Numerical:
    //already copied the value and subtype
    break;
  case Uncertain:
  case Textual:
    //already copied the image.
    break;
  case Wad: //copy preserving order
    ForKidsConstly(list){
      createChild(list.next());
    }
    break;
  } /* switch */
} // Storable::clone

void Storable::reparent(Storable &newparent){
  if(this!=nullptr) {
    if(type==Wad) {
      while(Storable *kid = wad.takeNth(0)) {//must take in order in case order is important to caller.
        kid->parent = &newparent;
        newparent.wad.append(kid);
      }
      parent->remove(ownIndex());
    } else {
      dbg("Tried to reparent a scalar node:%s to %s",fullName().c_str(),newparent.fullName().c_str());
    }
  }
} // Storable::reparent

void Storable::assignFrom(Storable&other){
  if(&other == nullptr) {
    return;                     //breakpoint, probably a pathological case.
  }
  switch(type) {
//  default:
  case Uncertain:
  //#JOIN
  case NotDefined:
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
    ForKids(list){
      Storable&kid(list.next());//from datum?
      Storable *older(nonTrivial(kid.name) ? other.existingChild(kid.name) : other.wad[kid.ownIndex()]);
      if(older) {
        kid.assignFrom(*older);
      } else {
        //wtf("missing node in assignFrom: this %s, other %s, node %s ", this->fullName(), other.fullName(), kid.name.empty() ? "(nameless)" : kid.name);
      }
    }
    break;
  } /* switch */
} // assignFrom

double Storable::setValue(double value, Storable::Quality quality){
  bool notifeye = number.setto(value);

  notifeye |= setQuality(quality);
  if(enumerated) {
    //if enumerized then leave the type as is and update text
    text = enumerated->token(value);
  } else {
    notifeye |= setType(Numerical);//todo:0 refine subtype of number
  }
  also(notifeye); //record changed, but only trigger on fresh change
  if(notifeye) {
    notify();
  }
  return value;
} // setValue

void Storable::setImageFrom(TextKey value, Storable::Quality quality){

  if(isTrivial()) { //don't notify or detect change, no one is allowed to watch an uninitialized node
    text = value;
    if(quality==Edited) {//mostly if Parsed don't set type to text, it needs to be checked for keywords.
      setType(Textual);
    }
    setQuality(quality);
    return;
  } else {//has been touched in some fashion
    bool notifeye = false;
    if(type==Numerical) {
      text = value;   //#bypass change detect here, just recording for posterity
      Cstr units;
      NumbericalValue formerly(number);
      switch(number.is){
      case NumericalValue::Truthy:
        number=text.cvt<bool>(false,&units);
        break;
      case NumericalValue::Counting:
        number=text.cvt<unsigned>(0,&units);
        //todo:1 check for KMG units
        break;
      case NumericalValue::Whole:
        number=text.cvt<int>(0,&units);
        //todo:1 check for KMG units
        break;
      case NumericalValue::Floating:
        number=text.cvt<double>(0.0,&units);
        //todo:1 report nontrivial units.
        break;
      }
      notifeye=number!=formerly;
    } else {
      notifeye = changed(text, value);
    }
    notifeye |= setQuality(quality);
//    //we could COA and check for Wadness here, but that would preclude surviving a particular trivial json defect.
    also(notifeye); //record changed,
    if(notifeye) {//but only trigger on fresh change
      notify();
    }
  }
} // setImageFrom

void Storable::setImage(const TextKey &value, Quality quality){
  setImageFrom(value, quality);
}

Cstr Storable::image(void){
  switch(type) {
  case Uncertain:
//  default:
  case Textual://#ignore warning, if we remove it we get a different warning.
    return text;

  case Numerical:
    if(enumerated) {
      return enumerated->token(number.as<unsigned>());//don't update text, this is much more efficient since enumerated is effectively static.
    } else {
      //set the internal image without triggering change detect
      char buffer[64 + 1];//enough for 64 bit boolean image
      CharFormatter formatter(buffer,sizeof(buffer));
      switch(number.is) {
      case NumericalValue::Truthy:
        text.copy(number.as<bool>() ? "1" : "0");
        break;
      case NumericalValue::Whole:
        formatter.printSigned(number.as<int>());
        text.copy(buffer);
        break;
      case NumericalValue::Counting:
        formatter.printUnsigned(number.as<unsigned>());
        break;
      case NumericalValue::Floating:
        text.copy(NumberFormatter::makeNumber(number));
        break;
      } // switch
      return text;
    }
  case Wad:
    text.take(NumberFormatter::makeNumber(numChildren()));
    return text;
  default:
  case NotDefined:
    return "(unknown)";
  } // switch
} // Storable::image

Cstr Storable::getText() const {
  return text.c_str();
} // image

void Storable::setDefault(TextKey value){
  if((q == Empty) || (q == Defaulted)) {
    setImage(value, Defaulted);
  }
}

bool Storable::operator ==(TextKey zs){
  return type == Textual && text == zs;
}

unsigned Storable::numLeaves() const {
  if(type==Wad) {
    unsigned count(0);
    ForKidsConstly(list){
      count += list.next().numLeaves();
    }
    return count;
  } else {
    return 1;
  }
} // Storable::numLeaves

ChainScanner<Storable> Storable::kinder(){
  return {wad};
}

ConstChainScanner<Storable> Storable::kinder() const {
  return ConstChainScanner<Storable>(wad);
}

void Storable::forChildren(sigc::slot<void,Storable &> action){
  ForKids(list){
    Storable&one(list.next());
    action(one);
  }
}

Storable *Storable::existingChild(TextKey childName){
  //nameless nodes might be mixed in with named ones:
  ForKids(list){
    Storable&one(list.next());
    if(one.name.is(childName)) {
      return &one;
    }
  }
  return nullptr;
}

const Storable *Storable::existingChild(TextKey childName) const {
  //nameless nodes might be mixed in with named ones:
  if(nonTrivial(childName)) { //added guard to make assignFrom( a wad) easier to code.
    ForKidsConstly(list){
      const Storable&one(list.next());
      if(one.name.is(childName)) {
        return &one;
      }
    }
    return nullptr;
  }
  //never got here! never asked for trivial name!
  return nullptr;//can't find a nameless child. Need special functions for that which include a search start index.
} // Storable::existingChild

Storable *Storable::findNameless(unsigned lastFound){
  while(lastFound++<wad.quantity()) {
    if(wad[lastFound]->name.empty()) {
      return wad[lastFound];//and user can use the index thereof to pass back to this for the next one.
    }
  }
  return nullptr;
}

Storable &Storable::getRoot(){
  Storable *searcher = this;

  while(searcher&&searcher->parent) {
    searcher = parent;
  }
  return *searcher;
}

unsigned Storable::setSize(unsigned qty){
  unsigned changes = 0;
  while(qty<wad.quantity()) {
    wad.removeLast();
    wadWatchers.emit(true,wad.quantity());
    --changes;
  }
  while(qty>wad.quantity()) {
    addChild("");
    ++changes;
  }
  return changes;
} // Storable::setSize

Storable *Storable::getChild(Sequence<Text> &progeny,bool autocreate){
  if(this==nullptr) {
    return nullptr;//detect recursion gone bad
  }

  Storable *searcher = this;

  while(progeny.hasNext()) {
    Text &lname = progeny.next();
    if(lname.cmp("..")==0) { //if progeny.next is .. then found=parent, continue loop
      searcher = searcher->parent;
      if(!searcher) {
        //we can't autocreate a root, it would leak if we tried.
        wtf("Storable::getChild asked to look above root");
        return nullptr;//we do NOT autocreate in this case.
      }
      continue;//look for next child in path
    }

    Index which = numericalName(lname.c_str());

    if(which.isValid()) {//pick node by number.
      if(searcher->wad.has(which)) {
        searcher = searcher->wad[which];
        continue;
      }
      if(autocreate) {
        searcher->setSize(which + 1);//0-based which
        searcher = searcher->wad[which];//setSize creates nodes.
        return searcher->getChild(progeny,true);//must recurse to not copy all the 'is it a number' logic
      }
      return nullptr;
    }

    if(Storable * found = searcher->existingChild(lname)) {
      searcher = found;
      continue;//look for next child in path
    }
    if(autocreate) {
      return searcher->addChild(lname).getChild(progeny,true);
    }
    return nullptr;
  }
  //path exhausted without an abnormal exit, so we must have found the child:
  return searcher;
} // Storable::getChild

Storable *Storable::findChild(TextKey path, bool autocreate){
  if(this==nullptr) {
    return nullptr;
  }
  DottedName genealogy(slasher.slash,path);
  ChainScanner<Text> progeny(genealogy.indexer());

  if(genealogy.bracket.after) {
    wtf("Storable findChild is ignoring trailing separator: [%s]",path);
  }

  Storable *searcher = (genealogy.bracket.before) ? &getRoot() : this;
  return searcher->getChild(progeny,autocreate);
} // findChild

/** creates node if not present.*/
Storable&Storable::child(TextKey childName){
  if(nonTrivial(childName)) {
    if(Storable *child = existingChild(childName)) {
      return *child;
    } else {
      return addChild(childName);
    }
  } else {//#duplicated code for debug.
    return addChild(childName);
  }
} // Storable::child

Storable&Storable::operator ()(TextKey name){
  return child(name);
}

Storable&Storable::operator [](unsigned ordinal){
  if(!has(ordinal)) {
    wtf("nonexisting child of %s referenced by ordinal %d (out of %d).",fullName().c_str(), ordinal, numChildren());
    dbg.dumpStack("nth child doesn't exist");
    addChild(""); //better than an NPE so deep in the hierarchy that we don't know where it comes from.
    return *wad.last();
  }
  return *wad.nth(ordinal);
}

const Storable &Storable::operator[](unsigned ordinal) const {
  if (!has(ordinal)) {
    wtf("nonexisting child of %s referenced by ordinal %d (out of %d).", fullName().c_str(), ordinal, numChildren());
    dbg.dumpStack("nth child doesn't exist, null this returned");
  }
  return *wad.nth(ordinal);
}


const Storable&Storable::nth(unsigned ordinal) const {
  if(!has(ordinal)) {
    wtf("nonexisting child referenced by ordinal %d (out of %d).", ordinal, numChildren());
  }
  return *wad.nth(ordinal);
}

Storable &Storable::nth(unsigned ordinal){
  if(!has(ordinal)) {
    wtf("nonexisting child referenced by ordinal %d (out of %d).", ordinal, numChildren());
  }
  return *wad.nth(ordinal);
}

unsigned Storable::indexOf(const Storable&node) const {
  return wad.indexOf(&node);
}

Storable&Storable::addChild(TextKey childName){
  Storable&noob(precreate(childName));

  return finishCreatingChild(noob);
}

Storable&Storable::createChild(const Storable&other,TextKey altname){
  Storable&noob(precreate(altname ? altname : other.name.c_str()));

  noob.clone(other);
  return finishCreatingChild(noob);
}

Storable&Storable::finishCreatingChild(Storable&noob){
  noob.index = wad.quantity();
  wad.append(&noob);
  wadWatchers(false,noob.index);//especially used by stored group to create when remote posts into this node.
  if(remote) {
    remote->add(noob);
  }
  return noob;
}

Storable&Storable::addWad(unsigned qty, Storable::Type type, TextKey name){
  Storable&noob(precreate(name));

  noob.presize(qty, type);
  return finishCreatingChild(noob);
}

void Storable::presize(unsigned qty, Storable::Type type){
  if(qty>numChildren()) {
    unsigned i = qty - numChildren();

    while(i-- > 0) {
      Storable&kid = addChild("");
      kid.setType(type);
      //and allow constructed default values to persist
      kid.setQuality(Defaulted); //#!# not using Empty as that often masks the type being set.
    }
  }
} // Storable::presize

bool Storable::remove(unsigned which){
  if(has(which)) {
    if(remote) {//#done here instead of destructor so that we don't send program shutdown deletions.
      remote->remove(*wad[which]);        //remote can figure out how to deal with a floating node.
    }
    wad.removeNth(which);//delete's object here
    //renumber children, must follow removal to make for-loop cute
    for(unsigned ci = wad.quantity(); ci-- > which; ) { //from last downto item newly dropped into 'which' slot
      --(wad[ci]->index);
    }
    also(true);
    notify(); //added this notify so that NodeEditor deletes can show on screen.
    return true;
  } else {
    return false;
  }
} // remove

bool Storable::removeChild(Storable&node){
  return remove(indexOf(node));
}

bool Storable::removeChild(Cstr name){
  Storable *moriturus = existingChild(name);
  return moriturus && remove(moriturus->index);
}

void Storable::filicide(bool notify){
  also(numChildren() != 0); //mark for those who poll changes...
  wad.clear(); //remove WITHOUT notification, we only call filicide when we are cloning
  if(notify) {
    watchers.send();
  }
}

void Storable::suicide(bool andDelete){
  if(parent) {
    parent->remove(ownIndex());
    if(andDelete) {
      delete this;
    }
  }
}


///////////////////////
StoredListReuser::StoredListReuser(Storable&node, unsigned wadding) : node(node), wadding(wadding), pointer(0){
  //#nada
}

Storable&StoredListReuser::next(){
  if(node.has(pointer)) {
    if(wadding) {
      node[pointer].presize(wadding);          //wadding is minimum size of nodes to be created.
    }
    return node[pointer++]; //expedite frequent case
  }
  ++pointer;
  if(wadding) {
    return node.addWad(wadding);
  } else {
    return node.addChild("");
  }
} // next

unsigned StoredListReuser::done(){
  //pointer is quantity that have been done
  //killer is init to number that exist
  //if they are equal then all is well.
  for(unsigned killer = node.numChildren(); killer-- > pointer; ) { //#efficient order, no shuffling of ones that will then also be whacked.
    node.remove(killer);
  }
  return pointer;
}

//////////////////////
Storable::Freezer::Freezer(Storable&node, bool childrenToo, bool onlyChildren) : childrenToo(childrenToo), onlyChildren(onlyChildren), node(node){
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

void Storable::Freezer::freezeNode(Storable&node, bool childrenToo, bool onlyChildren){
  if(!onlyChildren) {
    node.watchers.gate();
  }
  if(childrenToo) {
    node.childwatchers.gate();
  }
}
