#ifndef STORABLE_H
#define STORABLE_H

#include "safely.h"
#include "argset.h" //for arrays to mate to hardware structs
#include "chain.h" //wrap std::vector to cover its sharp pointy sticks.
#include "changemonitored.h"

#include "charscanner.h"
#include "enumerated.h"

#include "iterate.h"
#include "logger.h"
#include "numberformatter.h"
#include "sigcuser.h"
#include "textpointer.h"

//class used for keys
typedef TextPointer NodeName;

#if UseGlib
//#include "channel.h"
//#include "gatedsignal.h"
#endif

/**
 * non-volatile storage and transport mechanism.
 * In most instances some derivative of Stored will wrap each Storable to set its type and other attributes.
 *
 * Storable supports both polled change detection via ChangeMonitored and change notification callbacks via sigc.
 *
 * Note that a Stored object is stored in a file when saveAll() is called, whereas a Storable object that is not wrapped in Stored
 * will not be saved.
 *
 */

class Storable : public ChangeMonitored, SIGCTRACKABLE {
  friend class Stored; //access to q and the like.
  friend class StoredLabel; //ditto
  friend class StoredEnum;
public:

  static int instances;  //number of storables in existence.  For debug.

  enum Type {  //referring to the type of data in the node
    NotKnown,
    Numerical,
    Textual,
    Wad,  //any node that has children is a wad
  };

  enum Quality { //will use this during diffs
    Empty, //constructed but never referenced
    Defaulted, //created by attempted access
    Parsed, //parsed from stream
    Edited, //modified by program action
  };

  /** hook to force tree node to save all pending changes prior to output.
   * NB: this does not recurse for wads, the caller of this must recurse if the entity is a wad.
   * we may change that for convenience, but presently all callers of preSave had their own reasons to iterate. */
  SimpleSignal preSave;

  /** ignore this node (and its children) during change polling detection, only apply to stuff that is automatically reconstructed
   * or purely diagnostic. Doesn't affect change watching, only polling. */
  bool isVolatile;
protected:
  Type type;
  Quality q;
  double number;
  Ustring text;
public:
  //made public for sibling access, could hide it with some explicit sibling methods.
  /** used primarily for debugging, don't have to unwind stack to discover source of a wtf herein. */
  Storable *parent;
  /** whether items must be transfered to/from storage in the order they are in the wad. '{' vs '{' in json files. */
  bool strictlyOrdered;
protected:
  /** whether wad is dynamically reorganized to allow for binary search for a child.
   * the parser should not set this, only the present program logic should control it.*/
  bool sorted;
protected:
  /** cached coordinate of this item in its parent's wad, updated by parent when that parent reorganizes its wad.*/
  int index;
  /** children of this node */
  Chain<Storable> wad;
  /** set by StoredEnum when one is created, maintains parallel text.*/
  const Enumerated *enumerated; //expected to be a globally shared one
  mutable int recursionCounter = 0;
  mutable GatedSignal watchers; //# mutable so that we can freeze and thaw
  mutable GatedSignal childwatchers; //# mutable, see @watchers.

  /** calls watchers */
  void notify() const;
  void recursiveNotify() const;
private:
  /* non-copyable */
  Storable(const Storable &noncopyable); // non construction-copyable
  Storable &operator =(const Storable &noncopyable); // non copyable
  /**a piece of constructor. @param name is node name */
  Storable &precreate(NodeName name);
public:
  /** this is *almost* const, need to work on clone() to make it so.*/
  TextPointer name;
  void setName(NodeName name);
  /** @param isVolatile was added here to get it set earlier for debug convenience */
  Storable(NodeName name, bool isVolatile = false);
  Storable(bool isVolatile = false);

  virtual ~Storable();

  // functions that apply to all types
  // getters and setters
  /** @returns untyped pointer to this, handy for gui access.*/
  gpointer raw();
  /** @return whether the type actually changed */
  bool setType(Type newtype);
  Type getType() const;
  bool setQuality(Quality q);
  /** sets a labeling for a numeric value. NB: the pointer is cached in this class, the item better not be deletable! */
  void setEnumerizer(const Enumerated *enumerated);
  const Enumerated *getEnumerizer() const;
  bool convertToNumber(bool ifPure);
  // various predicate-like things
  bool isTrivial() const;
  bool is(Type ty) const;
  bool is(Quality q) const;
  bool isModified() const;
  // more involved functions
  bool wasModified();
  /** @return number of changes */
  int listModified(sigc::slot<void, Glib::ustring> textViewer) const;

  Glib::ustring fullName() const;
  /** the index is often not meaningful, but always is valid. It is -1 for a root node.*/
  int ownIndex() const {
    return index;
  }

  /** watch this node: if wad change is only adds and removes. */
  sigc::connection addChangeWatcher(const SimpleSlot &watcher, bool kickme = false) const;
  /** watch all children: (NB: can't unwatch individual kids) */
  sigc::connection addChangeMother(const SimpleSlot &watcher, bool kickme = false) const;
public:
  /** class added to guarantee thawing even in the face of exceptions*/
  class Freezer {
    bool childrenToo;
    bool onlyChildren;
    Storable &node;
public:
    Freezer(Storable &node, bool childrenToo = true, bool onlyChildren = false);
    ~Freezer();
    /** permenently freeze a node, such as when we are going to chop one up for export then discard it.*/
    static void freezeNode(Storable &node, bool childrenToo = true, bool onlyChildren = false);
  };

  /** make this node have same structure as givennode, but leave name and present children intact*/
  void clone(const Storable &other);
  /** like an operator = */
  void assignFrom(const Storable &other);
  /** set the value of a numerical node */
  double setValue(double value, Quality quality = Edited);
  // functions that apply to numbers
  /** sets numerical value, if node has an enumerated then the text is set to match, if no enumerated then node type is set to
   * numerical with gay disregard for its previous type. */
  template<typename Numeric> Numeric setNumber(Numeric value, Quality quality = Edited){
    setValue(static_cast<double>(value), quality);
    return static_cast<Numeric>(number);
  }

  template<typename Numeric> Numeric getNumber() const {
    return static_cast<Numeric>(number);
  }

  /** if not set from file or program execution then set a value on the node */
  template<typename Numeric> Numeric setDefault(Numeric value){
    if(q == Empty || q == Defaulted) {
      setNumber(value, Storable::Defaulted);
    }
    return getNumber<Numeric>();
  }

  /** @return a functor that when invoked will set this object's value to what is passed at this time.*/
  template<typename Numeric> sigc::slot<Numeric> getLater(){  //a sigc expert might be able to get rid of this and castTo.
    return MyHandler(Storable::getNumber<Numeric> );
  }

  /** hook up to send changes to a simple variable. */
  template<typename Numeric> sigc::connection sendChanges(Numeric &target, bool kickme = false){
    if(kickme) {
      target = getNumber<Numeric>();
    }
    return addChangeWatcher(sigc::bind(&assignTo<Numeric>, sigc::ref(target), getLater<Numeric>()));
  }

  // functions that apply to text
  void setImageFrom(const char *value, Quality quality = Edited);
  void setImage(const Glib::ustring &value, Quality quality = Edited);
  Glib::ustring image(void) const;
  void setDefault(const Glib::ustring &value);
  /** @return whether text value of node textually equals @param zs (at one time a null terminated string) */
  bool operator ==(const Glib::ustring &zs);

  /** @returns number of child nodes. using int rather than size_t to reduce number of casts required */
  int numChildren() const { //useful with array-like nodes.
    return wad.quantity();
  }

  /** @returns an iterator over the children, in ascending order*/
  ChainScanner<Storable> kinder();
  /** @returns an iterator over the children, in ascending order*/
  ConstChainScanner<Storable> kinder() const;

  bool has(int ordinal) const {
    return ordinal >= 0 && ordinal < numChildren();
  }

  /** @returns null pointer if no child by given name exists, else pointer to the child*/
  Storable *existingChild(NodeName childName);
  /** @see existingChild() non const version */
  const Storable *existingChild(NodeName childName) const;

  /** if @param autocreate is true then call child() on each piece of the @param path, else call existingChild() until either a
   * member is missing or the child is found.
   * FYI: tolerates null this! */
  Storable *findChild(NodeName path, bool autocreate = true); /* true as default is legacy from method this replaced.*/
  /** creates node if not present.*/
  Storable &child(NodeName childName);
  /** syntactic sugar for @see child(NodeName) */
  Storable &operator ()(NodeName name);
  //these give nth child
  Storable &operator [](int ordinal);
  const Storable &operator [](int ordinal) const;
  /** named version of operator [] const */
  const Storable &nth(int ordinal) const;
private:
  /** find the index of a child node. @returns -1 if not a wad or not found in the wad.*/
  int indexOf(const Storable &node) const;
public:
  /** add a new empty node */
  Storable &addChild(NodeName childName = "");
  Storable &addChild(Indexer<const char> childName);
  /** add a new node with content copied from existing one, created to clean up storedGroup entity copy*/
  Storable &createChild(const Storable &other);

  //combined presize and addChild to stifle trivial debug spew when adding a row to a table.
  Storable &addWad(int qty, Storable::Type type = NotKnown, NodeName name = "");
  void presize(int qty, Storable::Type type = NotKnown);
  /** remove a child of the wad, only makes sense for use with StoredGroup (or legacy cleanup) */
  bool remove(int which);
  bool removeChild(Storable &node);
  //  bool removeChild(const Glib::ustring &name);

  /** remove all children */
  void filicide();
  /** packs child values into the given @param args, if purify is true then argset entries in excess of the number of this node's
   * children are zeroed, else they are left unmodified  */
  void getArgs(ArgSet &args, bool purify = false);
  /** overwrite child nodes setting them to the given values, adding nodes as necessary to store all of the args.*/
  void setArgs(ArgSet &args);
private:
  Storable &finishCreatingChild(Storable &noob);
}; // class Storable

/** iterate over the children of given node (kinder is german  plural for child, like kindergarten) */
#define ForKinder(node) for(auto list(node.kinder()); list.hasNext(); )
#define ForKinderConstly(node) for(auto list(node.kinder()); list.hasNext(); )


/** usage as filter: sigc::bind(&byName, sigc::ref(name)) */
template<class Groupie> bool byName(const Glib::ustring &name, const Groupie & /*child*/, const Glib::ustring &seeking){
  return name == seeking;
}

/**
 * base class for interpreter of a storage node.
 * wrapper instead of extending Storable, to lighten each instance's memory footprint.
 * Only extend from this when program logic will alter values versus gui edit screens as the latter work directly on the nodes.*/
class Stored : SIGCTRACKABLE {
  Stored();
  Stored(const Stored &cantbecopied);
protected:
  /** used to per-class disable notification causing onParse' to be called before all children exist.*/
  bool duringConstruction;
  // you must add the following lines to your constructor, can't be a base class function as the virtual table isn't operational
  // yet:
  //    duringConstruction=false;
  //    onParse();
public:
  Storable &node;
  Stored(Storable &node);
  virtual ~Stored();
  /** hook for actions to perform when node is written to disk.
   * @returns success, if false then the write must be reported as partially failed.*/
  virtual void onPrint(){
  }

  //hook for actions when storage node is altered:
  virtual void onParse(){
  }

  /** needed to create slots in base class to call virtual function  */
  void doParse();
  const char *rawText() const;
  /** @param generation 0 is self, same as plain index() */
  int parentIndex(int generation = 1) const;
  int index() const;
  bool indexIs(int index) const;
  /** @return a functor that when called returns the present index of this item.*/
  sigc::slot<int> liveindex() const;


  /** used by stored group refresh operations, to track no-longer relevent items */
protected:
  bool refreshed;
public:
  virtual void prepRefresh(); //virtual to allow for additional pre-scan operations.
  void isRefreshed();
  /** created for use by StoredGroup::removeIf()*/
  bool notRefreshed() const;

  /** handy for diagnostics, and occasionally abused for feeding gui*/
  NodeName getName() const;
  void setName(NodeName name);
  //ArgSet stuff is interface to our hardware device protocol
  void getArgs(ArgSet &args);
  void setArgs(ArgSet &args);
  sigc::connection watchArgs(const SimpleSlot &watcher, bool kickme = false);

  void allocArgs(int qty);
  /** user wants children*/
  void getArgs(NodeName child, ArgSet &args);
  void setArgs(NodeName child, ArgSet &args);

  bool isEmpty() const;
  /** fire off node watchers */
  void triggerWatchers();
  /** @return slot that will call triggerWatchers. */
  SimpleSlot notifier();
  /** also works for watching all of the array elements of a StoredGroup of simple things.*/
  sigc::connection onAnyChange(SimpleSlot slotty, bool kickme = false);

  /** do not include this item in change detect, don't bother saving it if it is the only thing changed. */
  void markTrivial();

  /** @return type-free pointer to underlying storage node, handy for gui builder.*/
  gpointer raw() const {
    return static_cast<gpointer>(&node);
  }

  Glib::ustring image() const {
    return node.image();
  }

  /** for case of renamed child: upgrade this storage.
   * todo: make a weaker form which doesn't need a template arg by copying value members of oldnode according to typeinfo of new
   * node.*/
  template<typename Scalar> void legacy(const char *oldname, const char *newname){
    if(Storable * legacy = node.existingChild(oldname)) {
      node.child(newname).setNumber(legacy->getNumber<Scalar>());
      node.remove(legacy->index);
    }
  }

}; // class Stored

#define ConnectChild(varname, ...) varname(node.child( # varname ), ## __VA_ARGS__)
#define ConnectSibling(varname, ...) varname(node.parent->child( # varname ), ## __VA_ARGS__)

class StoredLabel : public Stored {
public:
  StoredLabel(Storable &node, const Glib::ustring &fallback = Glib::ustring());
  void setDefault(const Glib::ustring &deftext);
  const char *c_str() const;
  //this cast operator created "ambiguous overload" due to the various operator == methods.
  //  operator const char *() const{
  //    return c_str();
  //  }
  Glib::ustring toString() const;
  operator Glib::ustring() const {
    return toString();
  }

  bool isTrivial() const;
  void operator =(const StoredLabel &other);
  bool operator ==(const StoredLabel &other) const;
  void operator =(const Glib::ustring &zs);
  bool operator ==(const Glib::ustring &zs) const;
  void operator =(const char *zs);
  bool operator ==(const char *zs) const;

  /** calls the given slot with this.toString() as its argument */
  void applyTo(sigc::slot<void, const char *> slotty);
  /** on a change to the value will call applyTo with the given slot */
  sigc::connection onChange(sigc::slot<void, const char *> slotty);
  /** a slot that will set the value of this */
  sigc::slot<void, const char *> setter();
}; // class StoredLabel

/** auto creating iterator that provides for deleting the unscanned items.
 * todo:2 construction option to invoked done() in destructor. Handy for for loops, but not not always desired..*/
class StoredListReuser {
  Storable &node;
  int wadding;
  int pointer;
public:
  StoredListReuser(Storable &node, int wadding = 0);
  Storable &next();
  int done();
};

////////////////////////////////
//macros for use with StoredGroup
/** for multple references into the item: Groupie &item(list.next()); */
#define ForGroup(group) for(auto list((group).all()); list.hasNext(); )

/** for multple references into the item: const Groupie &item(list.next()); */
#define ForGroupConstly(group) for(auto list((group).all()); list.hasNext(); )

#define ForCount(group) for(int i = (group).quantity(); i-- > 0; )
#define ForCountInOrder(group) for(int i = 0, ForCountInOrder_end = (group).quantity(); i < ForCountInOrder_end; ++i)
/////////////////////////////////////

#include <functional>


/**
 * class Groupie must implement a constructor that takes a Storable&
 * todo:1 add arguments to the StoredGroup constructor that are then passed to each Groupie as it is instantiated. This will take
 * fancy variadic template work OR a class hierarchy with a derived class for each set of args. Or we could pass in a factory
 * functor
 *... defaulted to a templated creator function (whose syntax alh hasn't yet figured out).
 */
template<class Groupie> class StoredGroup : public Stored {
  Chain<Groupie> pod;
  /** bool remove (else add at end) , int*/
  sigc::signal<void, bool, int> dependents;

  /** some actions that might have been added to the dependents list need to finish before another set can be run (such as
   * subordinate creations) so in the absence of a priority mechanism in the signal we have an additional signal. Also many change
   * handlers only care about additions.*/
  sigc::signal<void, Groupie &> oncreation;
  /** for removal we want to have already destroyed object before calling some of the change watchers.*/
  sigc::signal<void, int> onremoval;
  /** before removal we ask for permission, need accumulating signal ...*/
  sigc::signal1<bool, Groupie &, AndUntilFalse> preremoval;
  /** before addition we ask for permission (triggers before 'oncreation')*/
  sigc::signal<bool> preaddition;

public:
  typedef sigc::slot<void, bool /*removing*/, size_t /* which*/> Watcher;
  /** attach a filter to determine if an item in the group is allowed to be removed */
  sigc::connection permissionToRemove(sigc::slot<bool, Groupie &> tester){
    return preremoval.connect(tester);
  }

  sigc::connection permissionToAdd(sigc::slot<bool> tester){
    return preaddition.connect(tester);
  }

public:
  /** the guy which is the index calls this on the indexed class passing the thing indexed.e.g. analytes.indexes(calsamples);
   * acquisitions.indexes(calsamples); indexedBy might be movable into a non-template base class but the syntax for calling it is
   * annoying.
   */

  //trying to work around some syntax stuff for indexedBy, and hey-why not add a feature of sorts?
  template<class PrimeContent> void createFor(PrimeContent &indexItem){
    create(indexItem.getName());
  }

  /** hooks up primary group (the operand) to manage allocation of this group's items which are 1:1 with the indexer group's items.
   * @see indexes for swapping the args for syntactic convenenience. */
  template<class PrimeContent> void indexedBy(StoredGroup<PrimeContent> &indexer){
    //by doing these instead of registering a dependent using (nor removed) onReorg all onAdditions take place before any
    // whenReorganized's are invoked, so dependent objects are all created before whenReorganized's are invoked. Must check that all
    // non-creation onAddition stuff doesn't need to wait.
    indexer.onRemoval(MyHandler(StoredGroup<Groupie>::remove));
    indexer.onAddition(MyHandler(StoredGroup<Groupie>::createFor<PrimeContent> ), false);
    setSize(indexer.quantity()); //at time of attachment we resize the indexed entity
  }

  typedef ChainScanner<Groupie> Scanner;
  typedef ConstChainScanner<Groupie> ConstScanner;

  /** "in class" macros for StoredGroup.
   * outside of StoredGroup use the iterator factory
   * beware that when using this macro you must invoke list.next() in every body else you will spin forever (ie no conditional
   * invocation of list.next())*/
#define ForValues(list)   for(Scanner list(pod); list.hasNext(); )
#define ForValuesConstly(list)   for(ConstScanner list(pod); list.hasNext(); )

  Scanner all(){
    return Scanner(pod);
  }

  ConstScanner all() const {
    return ConstScanner(pod);
  }

  /**added to suppress warnings on things that are too difficult to properly "index" */
  bool autocreate;

  StoredGroup(Storable &node) : Stored(node), autocreate(false){
    if(node.setType(Storable::Wad)) { //needed in case group is presently empty, so that proper change watching is set up.
      dbg("Empty group?");
    }
    for(int ni = 0; ni < node.numChildren(); ++ni) { //#must be in order
      wrapNode(node[ni]);
    }
  }

  /** every new must have a delete*/
  virtual ~StoredGroup(){
    pod.clear();
    //don't do this, we don't signal when the whole group is being ditched: removeAll();
    //we don't touch the Storable nodes, they will go away when their root is destructed.
  }

  int quantity() const {
    return pod.quantity();
  }

  bool has(int ordinal) const {
    return unsigned(ordinal) < unsigned(quantity()); //#cute trick, comparing unsigned makes negative be really big
  }

  /** first created for sake of 'created' notification handlers*/
  Groupie &last(){
    if(quantity() == 0) {
      wtf("asked for last member of empty set");
    }
    return *pod.last();
  }

  /** first created for sake of 'created' notification handlers*/
  const Groupie &last() const {
    if(quantity() == 0) {
      wtf("asked for last member of empty set");
    }
    return *pod.last();
  }

  Groupie &operator [](int ordinal){
    if(has(ordinal)) {
      return *pod[ordinal];
    }
    if(autocreate) { //this is a debug feature, when we have to many problems to fix this right now.
      create("autocreated"); //todo:M periodically try to get rid of this crutch.
      return last();
    } else {
      wtf("asked for non-existent member: %d out of %d of set", ordinal, quantity());
      dumpStack("StoredGroup::nth");
      if(!has(0)) {
        create("autocreated"); //better than a random crash.
      }
      return *pod[0];
    }
  } // []

  const Groupie &operator [](int ordinal) const {
    if(has(ordinal)) {
      return *pod[ordinal];
    }
    wtf("asked for non-existent member: %d out of %d of set", ordinal, quantity());
    dumpStack("StoredGroup::nth");
    return *pod[0]; //which will still blow if there are no entities at all.
  }

  /** tableeditor needs this syntax */
  StoredGroup<Stored> *basecast(){
    return reinterpret_cast<StoredGroup<Stored> *>(this); //#compiler cannot see that all template args must be derived from Stored.
  }

  /** @param listner will be called on each add or remove, and if @param addAllNow is true it will be called as an add for all
   * present entities.
   * @returns a connection object by which this callback registration can be cancelled. */
  sigc::connection whenReorganized(const Watcher &listner, bool addAllNow = false){
    if(addAllNow) {
      ForCountInOrder(*this){
        listner(false, i);
      }
    }
    return dependents.connect(listner);
  }

  sigc::connection onAddition(sigc::slot<void, Groupie &> action, bool addAllNow = false){
    if(addAllNow) {
      ForValues(list){
        action(list.next());
      }
    }
    return oncreation.connect(action);
  }

  sigc::connection onRemoval(sigc::slot<void, int> action, bool doAllNow = false){
    if(doAllNow) {
      for(int i = quantity(); i-- > 0; ) {
        action(i);
      }
    }
    return onremoval.connect(action);
  }

  /**add a new node and build a new thing from it.*/
  Groupie &create(NodeName prename = ""){
    if(preaddition.empty() || preaddition()) {
      Storable::Freezer autothaw(node, true, true); //#must NOT allow change watchers to execute on node add until we create the
                                                    // object that they may come looking for. Without this the change actions will
                                                    // execute before the new object exists instead of after.
      wrapNode(node.addChild(prename));
    }
    return last();
  }

  /** add a copy of an existing node, build a new thing from it and hence a copy of that thing.
   * generally that existing node is from some other instance of a group of the same type as this group*/
  Groupie &clone(const Groupie &extant){
    wrapNode(node.createChild(extant.node));
    return last();
  }

  /** cloning copier. @param eraseFirst else will append */
  void assignfrom(StoredGroup<Groupie> &other, bool eraseFirst = true){ //todo:2 add a filter.
    if(eraseFirst) {
      removeAll();
    }
    for(ConstScanner list(other.pod); list.hasNext(); ) {
      clone(list.next());
    }
  }

  /** create enough records that quantity() == size
   * @returns the number of additions (if positive) or removals (if negative), 0 on no change.*/
  int setSize(int size){
    int changes(0);

    while(quantity() < size) {
      ++changes;
      create(); //a tag inserted here shows up in too many reasonable places, such as all levels of measurement reports.
    }
    while(quantity() > size) { //pathological case.
      --changes;
      remove(quantity() - 1);
    }
    return changes;
  } // setSize

  /** remove something from given place in list. This DELETES the item, beware of use-after-free.*/
  virtual void remove(int which){
    if(has(which)) { //#while the pod and node can take care of bad indexes locally we don't want to do the notifies if the index is
                     // bad. And now preremoval depends on this check.
      if(preremoval(operator [](which))) {
        //We actually delete the objects before we signal removal so iterations show it already gone
        pod.removeNth(which); //deletes Stored entity
        node.remove(which); //deletes underlying node
        onremoval(which);
        dependents(true, which);
      }
    }
  } // remove

  /** remove something from given place in list. This DELETES the item, beware of use-after-free.*/
  void removeItem(Groupie &member){ //this method was CREATING a new node!
    remove(ordinalOf(&member));
  }

  /** caller better be damned sure the row is the storage node of a Groupie from this group.*/
  void removeAnonymously(gpointer node){
    remove(ordinalOf(*static_cast<Storable *>(node)));
  }

  /** remove any member for which the slot is true, @return quantity removed. This DELETES the item, beware of use-after-free.*/
  int removeIf(const sigc::slot<bool, Groupie &> &killit){
    int deaths = 0; //nice diagnostic versus a simple bool.
    Storable::Freezer autothaw(node, false); //since node watch doesn't know what is removed wait until possibly multiple removes
                                             // are done before triggering its watchers (important when some watchers are gui
                                             // redraws)

    for(int which = quantity(); which-- > 0; ) { //#keep reverse iteration, can do remove's with it.
      Groupie &victim(*pod[which]);
      if(killit( victim)) {
        ++deaths;
        remove(which); //#works well because we reverse iterate. (see bug #369)
      }
    }
    return deaths;
  } // removeIf

  static bool anything(Groupie &){
    return true;
  }

  int removeAll(){ //#remove one at a time so that reorg demons run
    return removeIf(&anything);
  }

  /** @return ordinal of first entity meeting @param predicate, -1 for none.*/
  int first(const sigc::slot<bool, const Groupie &> &predicate) const {
    ForValuesConstly(list){
      if(predicate(list.next())) {
        return list.ordinal() - 1; //#already bumped to next entry
      }
    }
    return -1;
  }

  static bool byObject(const Groupie &child, const Groupie *unit){
    return &child == unit;
  }

  int ordinalOf(const Groupie *unit) const {
    return first(sigc::bind(&byObject, unit));
  }

  static bool byNode(const Groupie &child, const Storable &childnode){
    return &child.node == &childnode;
  }

  int ordinalOf(const Storable &childnode) const {
    return first(sigc::bind(&byNode, sigc::ref(childnode)));
  }

  Groupie *findFirst(const sigc::slot<bool, Groupie &> &predicate){
    ForValues(list){
      Groupie &item(list.next());

      if(predicate(item)) {
        return &item;
      }
    }
    return nullptr;
  }

  const Groupie *findFirst(const sigc::slot<bool, const Groupie &> &predicate) const {
    ForValuesConstly(list){
      const Groupie &item(list.next());

      if(predicate(item)) {
        return &item;
      }
    }
    return nullptr;
  }

  Groupie *find(const Storable &childnode){
    return findFirst(sigc::bind(&byNode, sigc::ref(childnode)));
  }

  /** @returns address of entity whose internal name matches key, nullptr if such does not exist.
   * useful for legacy upgrades of known entities within a group, which is pretty much limited to factory defined files, never user
   * stuff */
  Groupie *existing(const char *key){
    Storable *child = node.existingChild(key);

    if(child) {
      return find(*child);
    } else {
      return nullptr;
    }
  }

  /** @returns node by internal name, creates one if it doesn't exist.
   * useful for legacy upgrades of known entities within a group, which is pretty much limited to factory defined files, never user
   * stuff */
  Groupie &child(const char *key){
    Groupie *child = existing(key);

    if(child) {
      return *child;
    } else {
      return create(key);
    }
  }

  virtual bool wasModified(){
    return node.wasModified();
  }

  /** for when all you need is the item: */
  void forEachItem(const sigc::slot<void, Groupie &> &action){
    ForValues(list){
      Groupie &item(list.next());

      action( item);
    }
  }

  /** @deprecated need to get rid of these as they generate warnings:*/
  void forEach(const sigc::slot<void, const Glib::ustring &, const Groupie &, int> &action) const {
    ForValues(list){
      Groupie &item(list.next());

      action(item.name, item, item.ownIndex());
    }
  }

  /** faster than using slot's when the method is simple enough:*/
  void forEach(void (Groupie::*method)(void)){
    ForValues(list){
      Groupie &item(list.next());

      (item.*method)();
    }
  }

  /** experimental, to see if syntax is tolerable: */
  void forall(std::function<void(Groupie &)> action){
    ForValues(list){
      Groupie &item(list.next());

      action(item);
    }
  }

  /** clears refresh flag on each item */
  void prepRefresh(){
    forEach(&Stored::prepRefresh);
  }

  bool refreshDone(){
    return removeIf(&Stored::notRefreshed);
  }

private:
  StoredGroup(const StoredGroup &notallowed); //no basis for knowing what node to start from.

  /** @param addee can not be const as the Groupie constructor often adds members (such as for version upgrades). */
  void wrapNode(Storable &addee){ //formerly was called 'instantiate'
    int which = addee.ownIndex();

    if(quantity() != which) {
      wtf("Expected new node to be at end of list");
    }
    Groupie &newbie(*new Groupie(addee)); //@DEL in remove(), which is in turn called by StoredGroup destructor
    pod.append(&newbie);
    oncreation(newbie);
    dependents(false, which);
  } // wrapNode

  /** when called via the dependents signal schedule an action:*/
  static void doLater(bool removal, int arg, Watcher watcher){
    doSoon(sigc::bind(watcher, removal, arg), 0, 1);
  }

}; // class StoredGroup

#define INDEXER(group) *group.basecast()

#endif // STORABLE_H
