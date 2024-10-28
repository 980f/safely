#ifndef STOREDGROUP_H
#define STOREDGROUP_H

#include "stored.h"
#include "chain.h"

////////////////////////////////
//macros for use with StoredGroup
/** for multple references into the item: Groupie &item(list.next()); */
#define ForGroup(group) for(auto list((group).all()); list.hasNext(); )

/** for multple references into the item: const Groupie &item(list.next()); */
#define ForGroupConstly(group) for(auto list((group).all()); list.hasNext(); )

#define ForCount(group) for(unsigned i = (group).quantity(); i-- > 0; )
#define ForCountInOrder(group) for(unsigned i = 0, ForCountInOrder_end = (group).quantity(); i < ForCountInOrder_end; ++i)
/////////////////////////////////////

//this is probably gratuitous since we don't seem to be able to get away from sigc, which has greater functionality:
#include <functional>  //for lambdas

void callme(bool removed,unsigned which);


/**
 * class Groupie must implement a constructor that takes a Storable&
 * todo:1 add arguments to the StoredGroup constructor that are then passed to each Groupie as it is instantiated. This will take
 * fancy variadic template work OR a class hierarchy with a derived class for each set of args. Or we could pass in a factory
 * functor
 *... defaulted to a templated creator function (whose syntax alh hasn't yet figured out).
 */
template<class Groupie> class StoredGroup : public Stored {
  /** Stored<> objects, typically 1:1 with some Storable node's children */
  Chain<Groupie> pod;

  /** before addition we ask for permission */
  sigc::signal<bool> preaddition;

  /** some actions that might have been added to the dependents list need to finish before another set can be run (such as
   * subordinate creations) so in the absence of a priority mechanism in the signal we have an additional signal. Also many change
   * handlers only care about additions.
   *
   * passes freshly created item to slot
   */
  sigc::signal<void, Groupie &> oncreation;

  /** before removal we ask for permission */
  sigc::signal1<bool, Groupie &, AndUntilFalse> preremoval;

  /** for removal we want to have already destroyed object before calling some of the change watchers.
   * so this tells you the number of the one that has already been removed. The object is already deleted and ordinals adjusted.
   */
  sigc::signal<void, unsigned> onremoval;


  /** bool remove (else add at end) , unsigned which
   * called *after* a creation or a removal
   * this doesn't reference the item as often the watcher just wants to find a sibling
   */
  sigc::signal<void, bool, unsigned> dependents;


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
    create(indexItem.getName());//we can't rename existing nodes when the indexer is attached, but at least new nodes will share names.
  }

  /** hooks up primary group (the operand) to manage allocation of this group's items which are 1:1 with the indexer group's items.
   * @see indexes for swapping the args for syntactic convenenience. */
  template<class PrimeContent> void indexedBy(StoredGroup<PrimeContent> &indexer){
    //by using these instead of registering a dependent using onReorg, all onAdditions take place before any
    // whenReorganized's are invoked, so dependent objects are all created before whenReorganized's are invoked. Must check that all
    // non-creation onAddition stuff doesn't need to wait.
    indexer.onRemoval(sigc::hide_return(MyHandler(StoredGroup<Groupie>::remove)));
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
    for(unsigned ni = 0; ni < node.numChildren(); ++ni) { //#must be in order
      wrapNode(node[ni]);
    }
    node.wadWatchers.connect(MyHandler(StoredGroup::backdoored));
//debug stuff:    node.wadWatchers.connect(&callme);
  }

  /** someone has just deleted the node one of our members is connected to, it must die quickly or there will be use-after-free faults.*/
  void backdoored(bool removed,unsigned which){
    if(removed) {
      //we can't use remove(which) as it asks for permission and it is too late to stop the process.
      pod.removeNth(which); //deletes Stored entity
      onremoval(which);   //high priority notifications
      dependents(true, which);//lower priority notifications
    } else {
      wrapNode(node.nth(which));
    }
  }

  /** every new must have a delete ;) */
  virtual ~StoredGroup(){
    pod.clear();
    //don't do this, we don't signal when the whole group is being ditched: removeAll();
    //we don't touch the Storable nodes, they will go away when their root is destructed.
  }

  unsigned quantity() const {
    return pod.quantity();
  }

  bool has(unsigned ordinal) const {
    return ordinal < quantity();
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

  Groupie &operator [](unsigned ordinal){
    if(has(ordinal)) {
      return *pod[ordinal];
    }
    if(autocreate) { //this is a debug feature, when we have too many problems to fix this right now.
      create("autocreated"); //todo:M periodically try to get rid of this crutch.
      return last();
    } else {
      wtf("asked for non-existent member: %d out of %d of set", ordinal, quantity());
      wtf.dumpStack("StoredGroup::nth");
      if(!has(0)) {
        create("autocreated"); //better than a random crash.
      }
      return *pod[0];
    }
  } // []

  const Groupie &operator [](unsigned ordinal) const {
    if(has(ordinal)) {
      return *pod[ordinal];
    }
    wtf("asked for non-existent member: %d out of %d of set", ordinal, quantity());
    wtf.dumpStack("StoredGroup::nth const");
    return *pod[0]; //which will still blow if there are no entities at all.
  }

  /** @returns entity with underlying name, creating it with defaults if it didn't exist.
   * NB: Usually group entities aren't named (name is empty). */
  Groupie &operator()(TextKey name){
    return operator[](child(name).node.ownIndex());
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

  sigc::connection onRemoval(sigc::slot<void, unsigned> action, bool doAllNow = false){
    if(doAllNow) {
      for(unsigned i = quantity(); i-- > 0; ) {
        action(i);
      }
    }
    return onremoval.connect(action);
  }

  /**add a new node and build a new thing from it.*/
  Groupie &create(TextKey prename = ""){
    if(preaddition.empty() || preaddition()) {
      Storable::Freezer autothaw(node, true, true); //#must NOT allow change watchers to execute on node add until we create the object that they may come looking for. Without this the change actions would  execute before the new object exists instead of after.
      auto &backer = node.child(prename);//making a node makes related Groupie via backdoor. Node is allowed to already exist, a possibly pathological case.
      return (*this)[backer.ownIndex()];//just in case we didn't actually create a new node.
    } else {
      return last();//if not allowed to create a node return something as best as we can.
    }
  }

  /** add a copy of an existing node, build a new thing from it and hence a copy of that thing.
   * generally that existing node is from some other instance of a group of the same type as this group*/
  Groupie &clone(const Groupie &extant,TextKey altname = nullptr){
    wrapNode(node.createChild(extant.node,altname));
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
  int setSize(unsigned size){
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
  virtual bool remove(unsigned which){
    if(has(which)) { //#while the pod and node can take care of bad indexes locally we don't want to do the notifies if the index is bad. And now preremoval depends on this check.
      if(preremoval(operator [](which))) {//if not vetoed
        node.remove(which); //deletes underlying node, which may have its own watchers, and which deletes the Groupie via backdoor();
        return true;
      }
    }
    return false;
  } // remove

  /** remove something from given place in list. This DELETES the item, beware of use-after-free.*/
  bool removeItem(Groupie &member){
    ONNULLREF(member,false);//#gtk does this.
    return remove(ordinalOf(&member));
  }

  /** caller better be damned sure the row is the storage node of a Groupie from this group.*/
  void removeAnonymously(void * node){
    remove(ordinalOf(*static_cast<Storable *>(node)));
  }

  /** remove any member for which the slot is true, @return quantity removed. This DELETES the item, beware of use-after-free.*/
  unsigned removeIf(const sigc::slot<bool, Groupie &> &killit){
    unsigned deaths = 0; //nice diagnostic versus a simple bool.

    //since node watch doesn't know what is removed wait until possibly multiple removes are done before triggering its watchers (important when some watchers are gui redraws)
    Storable::Freezer autothaw(node, false);
    for(unsigned which = quantity(); which-- > 0; ) { //#keep reverse iteration, can do remove's with it.
      Groupie &victim(*pod[which]);
      if(killit( victim)) {
        deaths += remove(which); //#works well because we reverse iterate.(see bug #369)
      }
    }
    return deaths;
  } // removeIf

  static bool anything(Groupie &){
    return true;
  }

  /** @returns quantity removed */
  unsigned removeAll(){ //#remove one at a time so that reorg demons run
    return removeIf(&anything);
  }

  /** @return ordinal of first entity meeting @param predicate, -1 for none.*/
  unsigned first(const sigc::slot<bool, const Groupie &> &predicate) const {
    ForValuesConstly(list){
      if(predicate(list.next())) {
        return list.ordinal() - 1; //#already bumped to next entry
      }
    }
    return BadIndex;
  }

  /** a predicate for finding an object that might be in this storedgroup */
  static bool byObject(const Groupie &child, const Groupie *unit){
    return &child == unit;
  }

  unsigned ordinalOf(const Groupie *unit) const {
    return first(sigc::bind(&byObject, unit));
  }

  /** a predicate for finding a child by its node */
  static bool byNode(const Groupie &child, const Storable &childnode){
    return &child.node == &childnode;
  }

  unsigned ordinalOf(const Storable &childnode) const {
    return first(sigc::bind(&byNode, sigc::ref(childnode)));
  }

  /** @returns nullptr or first entity that positively meets the predicate */
  Groupie *findFirst(const sigc::slot<bool, Groupie &> &predicate){
    ForValues(list){
      Groupie &item(list.next());

      if(predicate(item)) {
        return &item;
      }
    }
    return nullptr;
  }

  /** @returns nullptr or first entity that positively meets the predicate */
  const Groupie *findFirst(const sigc::slot<bool, const Groupie &> &predicate) const {
    ForValuesConstly(list){
      const Groupie &item(list.next());

      if(predicate(item)) {
        return &item;
      }
    }
    return nullptr;
  }

  /** @returns nullptr or first entity who wraps the @param given node */
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
  Groupie &child(const char *key, bool *isNoob = nullptr){
    Groupie *child = existing(key);
    if(isNoob) {
      *isNoob = (child==nullptr);
    }
    if(child) {
      return *child;
    } else {
      return create(key);
    }
  } // child

  virtual bool wasModified(){
    return node.wasModified();
  }

  /** for when all you need is the item: */
  void forEachItem(const sigc::slot<void, Groupie &> &action){
    ForValues(list){
      action( list.next());
    }
  }

  /** @deprecated need to get rid of these as they generate warnings:*/
  void forEach(const sigc::slot<void, const TextKey &, const Groupie &, unsigned> &action) const {
    ForValues(list){
      Groupie &item(list.next());

      action(item.name, item, item.ownIndex());
    }
  }

  /** faster than using slot's when the method is simple enough:*/
  void forEach(void (Groupie::*method)(void)){
    ForValues(list){
      (list.next().*method)();
    }
  }

  /** experimental, to see if syntax is tolerable: */
  void forall(std::function<void(Groupie &)> action){
    ForValues(list){
      action(list.next());
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
    unsigned which = addee.ownIndex();

    if(quantity() != which) {
      wtf("Expected new node to be at end of list");
    }
    Groupie &newbie(*new Groupie(addee)); //@DEL in remove(), which is in turn called by StoredGroup destructor
    pod.append(&newbie);
    oncreation(newbie);
    dependents(false, which);
  } // wrapNode

}; // class StoredGroup

#define INDEXER(group) *group.basecast()

#endif // STOREDGROUP_H
