#include "logger.h"
#include "storable.h"
#include "charformatter.h"

#include "segmentedname.h" //for debug reports
#include "dottedname.h"
//#include "pathparser.h" //for finding a child by a pathname

//this is not a class member so that we don't force pathparser.h on all users:
static const PathParser::Rules slasher('/',false,true);// '.' gives java property naming, '/' would allow use of filename classes. '|' was used for gtkwrappers access

using namespace sigc;

//the following are only usable within Storable
#define ForKidsConstly(list) for(ConstChainScanner<Storable> list(wad); list.hasNext(); )
#define ForKids(list) for(ChainScanner<Storable> list(wad); list.hasNext(); )

Storable::Storable(TextKey name, bool isVolatile) : isVolatile(isVolatile), type(NotKnown), q(Empty), number(0), parent(nullptr), index(-1), enumerated(nullptr), name(name){
//  ++instances;
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
  //#we don't check isvolatile here as volatile nodes are often of singular interest, they just aren't of general interest (should not trigger group watch).
  watchers.send();
  recursiveNotify();
}

void Storable::recursiveNotify() const {
  if(isVolatile) {//this check is one of the main reasons for existence of isVolatile, to indicate gratuitous or redundant nodes
    return;
  }
  if(parent) {
    parent->childwatchers.send();
    parent->recursiveNotify();
  }
}

Storable&Storable::precreate(TextKey name){
  setType(Wad); //if we are adding a child we must be a wad.
  if(q == Empty) {
    q = Defaulted;
  }
  also(!isVolatile); //we altered the number of entities contained
  Storable&noob(*new Storable(name, false)); //only parent needs volatile flag as that stops recursive looking at the children.
  noob.parent = this;
  return noob;
} // precreate

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
  if(changed(Storable::enumerated, enumerated)) {
    if(enumerated) {
      if((q >= Parsed) && is(Storable::Textual)) { //if already has a text value
        number = enumerated->valueOf(text.c_str()); //reconcile numeric value
      } else {
        if(type == NotKnown) {
          setType(Storable::Textual); //todo:1 probably should be numeric and numeric should check for presence of enumerated, or add a specific Storable::Enumerated to
        }
        // reduce redundant checks.
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
    return false;//already a number
  } else {//convert image to number,
    bool impure(true);
    double ifNumber(toDouble(text.c_str(), &impure));

    if(!ifPure || !impure) {//if we don't care if it is a pure number, or if it is pure
      setType(Storable::Numerical);
      setNumber(ifNumber, q);
      return true;
    } else {
      return false;
    }
  }
} // Storable::convertToNumber

bool Storable::resolve(bool recursively){
  if(is(Storable::Uncertain)) {
    if(convertToNumber(true)) {//if is an image of a pure number (no units text)
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
  return type == NotKnown || q == Empty;
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
    //  JOIN
  case Numerical:
  case Textual:
    return ChangeMonitored::isModified();
  default:
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
  case NotKnown:
    return false;

  case Wad: { //investigate all children:  //need bracing to keep 'changes' local.
    int changes = 0;   //only count node's own changed if no child is changed
    ForKids(list){
      if(list.next().wasModified()) {
        ++changes;
        //#don't exit early, we want to run wasModified() on all entities to clear their dirty bits.
      }
    }
    return changes > 0 || thiswas;
  }
    //  JOIN;
  case Numerical:
    //JOIN;
  case Textual:
    return thiswas;
  } // switch
} // wasModified

#if StorableDebugStringy
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
      const Storable&child(list.next());

      changes += child.listModified(textViewer);   //recurse
    }
    if(!changes && ChangeMonitored::isModified()) {   //try not to report propagated changes already reported by loop above.
      textViewer(fullName() + ":reorganized");
      ++changes;
    }
    return changes;
  }
  case Numerical:
  case Uncertain:
  case Textual:
    if(ChangeMonitored::isModified()) {
      textViewer(Ustring::compose("%1:%2", fullName(), image()));
      return 1;
    }
    return 0;
  } // switch
} // Storable::listModified

#endif // if StorableDebugStringy

Text Storable::fullName() const {
  //non-recursive,
  SegmentedName pathname;
  const Storable *scan = this;

  do {
    pathname.prefix(scan->name);
  } while((scan = scan->parent));

  return PathParser::pack(pathname,slasher);
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
  switch(other.type) {
  //trust compiler to bitch if case missing:--  default:
  case NotKnown:
    dbg("!Unknown node in tree being copied");
    return; //

  case Numerical:
    number = other.number;
    break;
  case Uncertain:
  case Textual:
    text.copy(other.text);//want independent copy
    break;
  case Wad: //copy preserving order
    for(ConstChainScanner<Storable> list(other.wad); list.hasNext(); ) {
      createChild(list.next());
    }
    break;
  } /* switch */
} // clone

void Storable::assignFrom(Storable&other){
  if(&other == nullptr) {
    return;                     //breakpoint, probably a pathological case.
  }
  switch(type) {
  case Uncertain:
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

void Storable::setImageFrom(TextKey value, Storable::Quality quality){
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

void Storable::setImage(const TextKey &value, Quality quality){
  setImageFrom(value, quality);
}

Cstr Storable::image(void){
  switch(type) {
  default:
  case Uncertain:
    resolve(false);
    //  JOIN;
  case Textual:
    return text;

  case Numerical:
    if(enumerated) {
      return enumerated->token(int(number));//don't update text, this is much more efficient since enumerated is effectively static.
    } else {
      //set the internal image without triggering change detect
      text.copy(NumberFormatter::makeNumber(number));
      return text;
    }
  case Wad:
    text.take(NumberFormatter::makeNumber(numChildren()));
    return text;

  case NotKnown:
    return "(unknown)";
  } // switch
} // image

void Storable::setDefault(TextKey value){
  if((q == Empty) || (q == Defaulted)) {
    setImage(value, Defaulted);
  }
}

bool Storable::operator ==(TextKey zs){
  return type == Textual && text == zs;
}

ChainScanner<Storable> Storable::kinder(){
  return ChainScanner<Storable>(wad);
}

ConstChainScanner<Storable> Storable::kinder() const {
  return ConstChainScanner<Storable>(wad);
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

Storable &Storable::getRoot() {
  Storable *searcher = this;

  while(searcher&&searcher->parent){
    searcher=parent;
  }
  return *searcher;
}

Storable *Storable::findChild(TextKey path, bool autocreate){
  if(this==nullptr){
    return nullptr;
  }
  DottedName genealogy(slasher.slash,path);
  Storable *searcher = (genealogy.bracket.before)?&getRoot():this;

  if(genealogy.bracket.after) {
    wtf("Storable findChild is ignoring trailing separator: [%s]",path);
  }

  auto progeny(genealogy.indexer());
  while(progeny.hasNext()) {
    Text &lname=progeny.next();
    if(lname.cmp("..")==0){  //if progeny.next is .. then found=parent, continue loop
      searcher=searcher->parent;
      if(!searcher){
        //we can't autocreate a root, it would leak if we tried.
        wtf("Storable::findChild asked to look above root [%s]",path);
        return nullptr;//we do NOT autocreate in this case.
      }
      continue;//look for next child in path
    }

    if(Storable * found = searcher->existingChild(lname)) {
      searcher = found;
      continue;//look for next child in path
    } else if(autocreate) {
      //build children expeditiously, could create one via lname and recurse, but that would entail repeat parsing of the path.
      progeny.rewind(1);//undo the next so we don't have to duplicate code using lname.
      while(progeny.hasNext()) {
        searcher = &(searcher->addChild(progeny.next()));
      }
      break;//created the child (and possibly a few parents as well)
    } else {
      return nullptr;
    }
  }

  //path exhausted without an abnormal exit, so we must have found the child:
  return searcher;
} // findChild

/** creates node if not present.*/
Storable&Storable::child(TextKey childName){
  if(Storable *child = existingChild(childName)) {
    return *child;
  }
  return addChild(childName);
}

Storable&Storable::operator ()(TextKey name){
  return child(name);
}

Storable&Storable::operator [](int ordinal){
  if(!has(ordinal)) {
    wtf("nonexisting child of %s referenced by ordinal %d (out of %d).",fullName().c_str(), ordinal, numChildren());
    dbg.dumpStack("nth child doesn't exist");
    addChild(""); //better than an NPE so deep in the hierarchy that we don't know where it comes from.
    return *wad.last();
  }
  return *wad[ordinal];
}

const Storable&Storable::nth(int ordinal) const {
  if(!has(ordinal)) {
    wtf("nonexisting child referenced by ordinal %d (out of %d).", ordinal, numChildren());
  }
  return *wad[ordinal];
}

unsigned Storable::indexOf(const Storable&node) const {
  return wad.indexOf(&node);
}

Storable&Storable::addChild(TextKey childName){
  Storable&noob(precreate(childName));

  return finishCreatingChild(noob);
}

Storable&Storable::createChild(const Storable&other){
  Storable&noob(precreate(other.name));

  noob.clone(other);
  return finishCreatingChild(noob);
}

Storable&Storable::finishCreatingChild(Storable&noob){
  noob.index = wad.quantity();
  wad.append(&noob); //todo:sorted insert
  return noob;
}

Storable&Storable::addWad(int qty, Storable::Type type, TextKey name){
  Storable&noob(precreate(name));

  noob.presize(qty, type);
  return finishCreatingChild(noob);
}

void Storable::presize(int qty, Storable::Type type){
  int i = qty - numChildren();

  while(i-- > 0) {
    Storable&kid = addChild("");
    kid.setType(type);
    //and allow constructed default values to persist
    setQuality(Defaulted); //not using Empty as that often masks the type being set.
  }
}

bool Storable::remove(int which){
  if(wad.removeNth(which)) {//if something was actually removed
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

void Storable::getArgs(ArgSet&args, bool purify){
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

void Storable::setArgs(ArgSet&args){
  while(args.hasNext()) {
    int which = args.ordinal();
    if(has(which)) {
      wad[which]->setNumber(args.next());
    }
  }
}

///////////////////////
StoredListReuser::StoredListReuser(Storable&node, int wadding) : node(node), wadding(wadding), pointer(0){
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

