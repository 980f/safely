#ifndef STORABLE_H
#define STORABLE_H

#include "safely.h"

#if StoreArgsFeatured
#include "argset.h" //for arrays to mate to hardware structs
#endif

#include "chain.h" //wrap std::vector to cover its sharp pointy sticks.
#include "changemonitored.h"

#include "enumerated.h"

//#include "iterate.h"
#include "logger.h"
#include "numberformatter.h"
#include "sigcuser.h"

#include "gatedsignal.h"

#include "textpointer.h"

//class used for communicating keys, once was also for the actual storage.
typedef TextKey NodeName;

//class for text value storage.
typedef Text TextValue;

/**
 * non-volatile storage and transport mechanism.
 * In most instances some derivative of Stored will wrap each Storable to set its type and other attributes.
 *
 * Storable supports both polled change detection via ChangeMonitored and change notification callbacks via sigc.
 *
 * Note that a Stored object is stored in a file when saveAll() is called, whereas a Storable object that is not wrapped in Stored
 * will not be saved.
 *
 * Made sigctrackable as these are often the objects of watched updates.
 *
 */

class Storable : public ChangeMonitored, SIGCTRACKABLE {
  friend class Stored; //access to q and the like.
  friend class StoredLabel; //ditto
  friend class StoredEnum;
public:
  /** path seperator for looking up child by name */
  static const char slasher;
  /** number of storables in existence.  For debug of memory leaks. */
  static int instances;

  enum Type {  //referring to the type of data in the node
    NotKnown,   //construction error, parse error
    Numerical, //asked to be a number, converts image to number when this is set
    Uncertain, //parsed, not yet evaluated as to type (defering a step of the original parser to lazyinit code in setType.
    Textual,   //asked to be text, or was found quoted during parse
    Wad,  //any node that has children is a wad
  };

  enum Quality { //will use this during diffs
    Empty, //constructed but never referenced
    Defaulted, //created by attempted access
    Parsed, //parsed from stream
    Edited, //modified by program action
  };

  /** ignore this node (and its children) during change polling detection, only apply to stuff that is automatically reconstructed
   * or purely diagnostic. Doesn't affect change watching, only polling. */
  bool isVolatile;

  /** hook to force tree node to save all pending changes prior to output.
   * NB: this does not recurse for wads, the caller of this must recurse if the entity is a wad.
   * we may change that for convenience, but presently all callers of preSave had their own reasons to iterate.
   *  This is essential for Stored's functioning.
   */
  SimpleSignal preSave;

private: //#the watchers must be mutable because sigc needs to be able to cull dead slots from its internal lists.
  /** sent when value changes, or quantity of wad changes*/
  mutable GatedSignal watchers; //# mutable so that we can freeze and thaw
  /** sent when any child changes, allows setting a watch on children that may not exist. */
  mutable GatedSignal childwatchers; //# mutable, see @watchers.

//was hunting a bug  mutable int recursionCounter = 0;

protected:
  /** stored value is like a union, although we didn't actually use a union so that a text image of the value can be maintained for debug of parsing and such. */
  Type type;
  /** for debug of file read and write stuff, not used in application logic (as far as we know) */
  Quality q;
  /** value if type is numeric or enum */
  double number;
  /** value if type is textual or enum, also used for class diagnostics */
  TextValue text;
public:   //made public for sibling access, could hide it with some explicit sibling methods.
  /** used primarily for debugging, don't have to unwind stack to discover source of a wtf herein. */
  Storable *parent;
protected:
  /** cached coordinate of this item in its parent's wad, updated by parent when that parent reorganizes its wad.*/
  int index;
  /** children of this node */
  Chain<Storable> wad;
  /** set by StoredEnum when one is created, maintains parallel text.*/
  const Enumerated *enumerated; //expected to be a globally shared one


  /** calls watchers */
  void notify() const;
  void recursiveNotify() const;
private:
  /* non-copyable */
  Storable(const Storable &noncopyable); // non construction-copyable
  Storable &operator =(const Storable &noncopyable); // non copyable
  /**a piece of constructor. @param name is node name */
  Storable &precreate(TextKey name);
public:
  /** this is *almost* const, need to work on clone() to make it so.*/
  const TextValue name;
private:
  void setName(TextKey name);
public:
  /** @param isVolatile was added here to get it set earlier for debug convenience */
  Storable(TextKey name, bool isVolatile = false);
  Storable(bool isVolatile = false);
//todo: why is this virtual? does sigc need that? we shouldn't derive from Storable, we wrap it.
  virtual ~Storable();

  // functions that apply to all types
  // getters and setters
  /** @returns untyped pointer to this, handy for gtk gui access.*/
  void * raw();
  /** @return whether the type actually changed */
  bool setType(Type newtype);
  Type getType() const;
  bool setQuality(Quality q);
  /** sets a labeling for a numeric value. NB: the pointer is cached in this class, the item better not be deletable! */
  void setEnumerizer(const Enumerated *enumerated);
  const Enumerated *getEnumerizer() const;
  /** @returns whether the text value was converted to a number. @param ifPure is whether to restrict the conversion to strings that are just a number, or whether
   * trailing text is to be ignored. */
  bool convertToNumber(bool ifPure);
/** convert an Unknown to either Numerical or Text depending upon purity, for other types @returns false */
  bool resolve(bool recursively);

  // various predicate-like things
  bool isTrivial() const;
  bool is(Type ty) const;
  bool is(Quality q) const;
  bool isModified() const;
  // more involved functions
  bool wasModified();

#if StorableDebugStringy
  /** @return number of changes */
  int listModified(sigc::slot<void, Ustring> textViewer) const;
#endif
  Text fullName() const;

  /** the index is often not meaningful, but always is valid. It is -1 for a root node.*/
  int ownIndex() const {
    return index;
  }

  /** watch this node's value, if wad the only change here is adds and removes. */
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
private: //@deprecated, need use case
  void clone(const Storable &other);
public:
  /** like an operator =
   *  rhs is not const due to image() mutating the text when not Textual */
  void assignFrom(Storable &other);
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
  void setImageFrom(TextKey value, Quality quality = Edited);
  void setImage(const TextKey &value, Quality quality = Edited);
  /** this method is not const as we lazily reuse text for image of non-text instances */
  Cstr image(void);
  void setDefault(TextKey value);
  /** @return whether text value of node textually equals @param zs (at one time a null terminated string) */
  bool operator ==(TextKey zs);

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
  Storable *existingChild(TextKey childName);
  /** @see existingChild() non const version */
  const Storable *existingChild(TextKey childName) const;

  /** find nameless nodes, starting at &param lastFound. Pass ~0 for 'beginning`.
   * To walk the list:
   * for(Storable *nemo=node.findNameless();nemo;nemo=node.findNameless(nemo.ownIndex())) dosomething(nemo); //nemo will not be null
   * @deprecated untested
   */
  Storable *findNameless(int lastFound = ~0);

  /** if @param autocreate is true then call child() on each piece of the @param path, else call existingChild() until either a
   * member is missing or the child is found.
   * FYI: tolerates null this! */
  Storable *findChild(TextKey path, bool autocreate = true); /* true as default is legacy from method this replaced.*/

  /** creates node if not present.*/
  Storable &child(TextKey childName);

  /** syntactic sugar for @see child(NodeName) */
  Storable &operator ()(TextKey name);
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
  Storable &addChild(TextKey childName);//removed default, nameless nodes are rare, make them stand out.

  /** add a new node with content copied from existing one, created to clean up storedGroup entity copy*/
  Storable &createChild(const Storable &other);

  //combined presize and addChild to stifle trivial debug spew when adding a row to a table.
  Storable &addWad(int qty, Storable::Type type = NotKnown, TextKey name = "");
  void presize(int qty, Storable::Type type = NotKnown);

  /** remove a child of the wad, only makes sense for use with StoredGroup (or legacy cleanup) */
  bool remove(int which);
  bool removeChild(Storable &node);
  bool removeChild(Cstr name);

  /** remove all children */
  void filicide(bool notify = false);
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

#endif // STORABLE_H
