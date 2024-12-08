#ifndef STORED_H
#define STORED_H

#include "storable.h"

/**
 * base class for interpreter of a storage node.
 * wrapper instead of extending Storable, to lighten each instance's memory footprint.
 **/
class Stored : SIGCTRACKABLE {
  Stored() = delete;//# we must attach to a storable, we exist to wrap access to one with type-safety.
  Stored(const Stored &cantbecopied) = delete;//can't copy a subset of a tree, not generically.
protected:
  /** used to per-class disable notification causing onParse' to be called before all children exist.
   * Only a few situations have needed to do this.
   */
  bool duringConstruction;
  // you must add the following lines to your constructor, can't be a base class function as the virtual table isn't operational yet:
  //    duringConstruction=false;
  //    onParse();

public:
  /** being a reference there is little danger in exposing it. It was handy having it available without the extra () of a getter.*/
  Storable &node;
  /** the essential constructor. */
  Stored(Storable &node);
  /** sigc needs this, you probably do for other reasons as well. */
  virtual ~Stored();
  /** hook for actions to perform prior to export (do any deferred updates) */
  virtual void onPrint(){
  }

  /** hook to cache anything dependent upon underlying storage, probably should deprecate and use watchers.*/
  virtual void onParse(){
  }

  /** this is needed to create slots in base class to call onParse due to onParse being a virtual function and the vtable not usable at the time that sigc needed it to be.
   * IE virtual functions are not usable by sigc in the base class constructor of that class- you only get the base class version */
  void doParse();
  /** pointer to text value's first char, dangerous! here for some GUI access. */
  TextKey rawText() const;
  /** @param generation 0 is self, same as plain index()
   *  @returns ordinal within parent wad of this item. Useful for parallel array stuff */
  unsigned parentIndex(int generation = 1) const;
  /** @returns ordinal of the wrapped node.  */
  unsigned index() const;
  /** @returns whether this node's ordinal is @param index */
  bool indexIs(unsigned index) const;
  /** @returns a functor that when called returns the present index of this item.*/
  sigc::slot<unsigned()> liveindex() const;

/** @returns whether underlying storage node is named per @param name */
  bool isNamed(TextKey name);

  /** The next stuff is used by stored group refresh operations, to track no-longer relevent items, kinda like a mark-sweep GC. */
protected:
  bool refreshed; //todo: isolate into helper class, or otherwise allow for 'refresh' to be conditionally compiled
public:
  virtual void prepRefresh(); //virtual to allow for additional pre-scan operations.
  void isRefreshed();
  /** created for use by StoredGroup::removeIf()*/
  bool notRefreshed() const;
//end refresh logic.

  /** @returns *copy* of underlying node's name. Since the node name is const as of late 2016 this will stay the name, but manipulating the returned value will not alter the node's
   * name. */
  TextKey getName() const;

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
  void *raw() const {
    return static_cast<void *>(&node);
  }

  /** @returns nominally constant text image.
   * IN a prior implementation this was a connection to an editable item, we now force old-timey seperate read and write.
   * I.E. you cannot alter the content without using setImage() and triggering watchers. When this returned a reference to an intelligent string class the change detect
   * was potentially bypassed.
   */
  Cstr image() const {
    return node.image();
  }

  /** for case of renamed child: upgrade this storage.
   * @deprecated use non template form so that string values can also be 'legacy'
   * node.*/
  template<typename Scalar> void legacy(TextKey oldname, TextKey newname){
    if(Storable * legacy = node.existingChild(oldname)) {
      node.child(newname).setNumber(legacy->getNumber<Scalar>());
      node.remove(legacy->index);
    }
  }

  /** handle rename of a member of a Stored-dreivative. Does NOT deal with moving something up or down a level, but you can copy that logic manually */
  void legacy(TextKey oldname, TextKey newname, bool purgeOld = true);

}; // class Stored


/** the ConnectChid macro is the main usablity feature of this class ensemble.
 * In any derived class of Stored one must have a constructor that takes a Storable.
 * If you label that argument 'node' then for each Stored-derived member of your Stored-derived class the ConnectChild macro will find a node named the same as the variable within the enclosing class's node and use that to init it. This uses var-args for additional construction arguments which are usually default values. */
#define ConnectChild(varname, ...) varname(node.child( # varname ), ## __VA_ARGS__)

//This seems to be a legacy thing, if you find a good reason to use it please document that here.
#define ConnectSibling(varname, ...) varname(node.parent->child( # varname ), ## __VA_ARGS__)
//use this to construct an object which is not a member of a Stored:
#define ConnectGroot(varname,...)  varname(Storable::Groot( # varname ), ## __VA_ARGS__)

/** for usage as filter: sigc::bind(&byName, sigc::ref(name)) */
template<class Groupie> bool byName(const TextKey &name, const Groupie & /*child*/, const Text &seeking){
  return seeking == name;
}

#endif // STORED_H
