#ifndef STORABLE_H
#define STORABLE_H

//#include "safely.h"

//#include "argset.h" //for arrays to mate to hardware structs

#include "chain.h" //wrap std::vector to cover its sharp pointy sticks.
#include "changemonitored.h"

#include "enumerated.h"

#include "logger.h"
#include "numberformatter.h"
#include "sigcuser.h"

#include "gatedsignal.h"

#include "textpointer.h"
#include "numericalvalue.h"

//class for text value storage.
typedef Text TextValue;

/**
 * non-volatile key-value storage and transport mechanism.
 * The key is text and is non-mutable from outside the class.
 *
 * In most instances some derivative of Stored will wrap each Storable to set its type and other attributes.
 *
 * Storable supports both polled change detection via ChangeMonitored and change notification callbacks via sigc.
 *
 * Made sigctrackable as these are often the objects of watched updates.
 *
 * todo:1 writing a string to a wad creates a child by that name.
 * todo:1 writing a negative number to a wad removes the indicated child
 * todo:1 writing a positive number to a wad increase its size to that value.
 */

class Storable : public ChangeMonitored, SIGCTRACKABLE {
  friend class Stored; //access to q and the like.
  friend class StoredLabel; //ditto
  friend class StoredEnum;

  friend class StorageWalker;

  //needed to be a template to befriend any StoredGroup
  template<typename> friend class StoredGroup;
public:
  /** the type of data in the node */
  enum Type { //#ordered for certain tests
    NotDefined,   //construction error, parse error
    Uncertain, //parsed, not yet evaluated as to type (defering a step of the original parser to lazyinit code in setType).
    Textual,   //asked to be text, or was found quoted during parse
    Numerical, //asked to be a number, converts image to number when this is set
    Wad,  //any node that has children is a wad
  };

  /** for debugging application logic, e.g. if Edited then it has changed since loaded by a file or set by default. */
  enum Quality {
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
  /** sent when value changes */
  mutable GatedSignal watchers; //# mutable so that we can freeze and thaw
  /** sent when any child's value changes, allows setting a watch on children that may not exist at the time of registration of the watcher. */
  mutable GatedSignal childwatchers; //# mutable, see @watchers.
public: //needed only by StoredGroup, but that being a template made friending it difficult.
  /** bool remove (else add at end) , unsigned which
   * called after an item is added or before it is removed.
   * First needed by socket access for remote editing of StoredGroups.
   */
  mutable  sigc::signal<void, bool, unsigned> wadWatchers;

protected:
  /** stored value is like a union, although we didn't actually use a union so that a text image of the value can be maintained for debug of parsing and such. */
  Type type;
  /** for debug of file read and write stuff, not used in application logic (as far as we know) */
  Quality q;
  /** value if type is numeric or enum */
  NumericalValue number;
  /** value if type is textual or enum, also used for class diagnostics */
  TextValue text;
protected: //if you need access add a method
  /** used primarily for debugging, don't have to unwind stack to discover source of a wtf herein. */
  Storable *parent;
protected:
  /** cached coordinate of this item in its parent's wad, updated by parent when that parent reorganizes its wad.*/
  unsigned index;
  /** children of this node */
  Chain<Storable> wad;
  /** set by StoredEnum when one is created, maintains parallel text.*/
  const Enumerated *enumerated; //expected to be a globally shared one

  /** calls watchers */
  void notify() const;
  void recursiveNotify() const;
public://publicly proclaim not accessible, priority of error messsages changed over time.
  /* non-copyable */
  Storable(const Storable &noncopyable)=delete;
  Storable &operator =(const Storable &noncopyable)=delete;
private:
  /** a piece of constructor. @param name is node name */
  Storable &precreate(TextKey name);
public:
  //had to change to Text vs saving a pointer when file loading comes first, else the file content gets ripped out from under us and we are pointing to reclaimable heap. It still is a good idea to not rename nodes, unless perhaps the name is empty.
  const Text name;
  /** somehow rename the node, perhaps by clone and replace */
  void Rename(TextKey newname);
public:
  /** @param isVolatile was added here to get it set earlier for debug convenience */
  explicit Storable(TextKey name, bool isVolatile = false);
  explicit Storable(bool isVolatile = false);

  //virtual for sigc
  virtual ~Storable();

  /** @returns untyped pointer to this, handy for gtk gui access.*/
  void * raw();

  /** @return whether the type actually changed */
  bool setType(Type newtype);
  Type getType() const;
  bool setQuality(Quality q);
  /** sets a labeling for a numeric value. NB: the pointer is cached in this class, the enumerizer better not be deletable! */
  void setEnumerizer(const Enumerated *enumerated);
  const Enumerated *getEnumerizer() const;

  /** @returns whether the text value was converted to a number.  */
  bool convertToNumber(NumericalValue::Detail numtype=NumericalValue::Detail::Floating);
/** convert an Unknown to either Numerical or Text depending upon purity, for other types @returns false */
  bool resolve(bool recursively);

  // various predicate-like things
  bool isTrivial() const;
  bool is(Type ty) const;
  bool is(Quality q) const;
  bool isModified() const override;
  // more involved functions
  bool wasModified() override;

  Text fullName() const;

  /** the index is often not meaningful, but always is valid. It is -1 for a root node.*/
  unsigned ownIndex() const {
    return index;
  }
  /** parent (0) is self, return own index ,if a member of a StoredGroup then this is index within group
   *  parent (1) is node containing the node of interest*/
  unsigned parentIndex(int generations) const; // parentIndex

  /** watch this node's value, if wad the only change here is adds and removes. */
  sigc::connection addChangeWatcher(const SimpleSlot &watcher, bool kickme = false) const;
  /** watch all children: (NB: can't unwatch individual kids) */
  sigc::connection addChangeMother(const SimpleSlot &watcher, bool kickme = false) const;
public:
  /** guarantee thawing even in the face of exceptions.
   * suspends notifications for the duration of existence of this object. */
  class Freezer {
    bool childrenToo;
    bool onlyChildren;
    Storable &node;
public:
    explicit Freezer(Storable &node, bool childrenToo = true, bool onlyChildren = false);
    ~Freezer();
    /** permenently freeze a node, such as when we are going to chop one up for export then discard it.*/
    static void freezeNode(Storable &node, bool childrenToo = true, bool onlyChildren = false);
  };


public:
  /** @deprecated, need use case.
   * make this node have same structure as givennode, but leave name and present children intact*/
  void clone(const Storable &other);
public://users of clone:
  /** replaces 'clone and remove'*/
  void reparent(Storable &newparent);
public:
  /** like an operator =
   *  rhs is not constable due to image() mutating the text when not Textual */
  void assignFrom(Storable &other);

  /** set the value of a numerical node */
  double setValue(double value, Quality quality = Edited);
  /** sets numerical value, if node has an enumerated then the text is set to match, if no enumerated then node type is set to
   * numerical with gay disregard for its previous type. */
  template<typename Numeric=double> Numeric setNumber(Numeric value, Quality quality = Edited){
    setValue(static_cast<double>(value), quality);
    return number;
  }

  void setNumber(NumericalValue other){
    number=other;
  }

  template<typename Numeric=double> Numeric getNumber() const {
    return number;
  }

  /** if no value has been set from parsing a file or program execution then set a value on the node. Defaults are normally set via ConnectChild macro. */
  template<typename Numeric> Numeric setDefault(Numeric value){
    if(q == Empty || q == Defaulted) {
      setNumber(value, Storable::Defaulted);
    }
    return getNumber<Numeric>();
  }

  /** @return a functor that when invoked will get this object's value. */
  template<typename Numeric> sigc::slot<Numeric> getLater(){
    return MyHandler(Storable::getNumber<Numeric> );
  }

  /** assigns any new value to @param target. If @param kickme is true does an immediate assignment of the present value. */
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
  Cstr image();
 /** this uses whatever the text presently is, if stale then you will have to find out why. */
 Cstr getText() const;

  void setDefault(TextKey value);

  /** @return whether text value of node textually equals @param zs (at one time a null terminated string) */
  bool operator ==(TextKey zs);

  /** @returns number of child nodes. using unsigned rather than size_t to reduce number of casts required */
  unsigned numChildren() const { //useful with array-like nodes.
    return wad.quantity();
  }

  /** just for fun- count the number of actual values in this node and its children. */
  unsigned numLeaves() const;

  /** @returns an iterator over the children, in ascending order*/
  ChainScanner<Storable> kinder();

  /** @returns an iterator over the children, in ascending order*/
  ConstChainScanner<Storable> kinder() const;

  /** experimental, to see if syntax is tolerable: */
  void forChildren(sigc::slot<void, Storable &> action);

  bool has(unsigned ordinal) const {
    return ordinal < numChildren();
  }

  /** @returns null pointer if no child by given name exists, else pointer to the child*/
  Storable *existingChild(TextKey childName);

  /** @see existingChild() non const version */
  const Storable *existingChild(TextKey childName) const;

  /** find nameless nodes, starting at &param lastFound. Pass BadIndex for 'beginning`.
   * To walk the list:
   * for(Storable *nemo=node.findNameless();nemo;nemo=node.findNameless(nemo.ownIndex())) dosomething(nemo); //nemo will not be null
   * @deprecated untested
   */
  Storable *findNameless(unsigned lastFound = BadIndex);

  /** if @param autocreate is true then call child() on each piece of the @param path, else call existingChild() until either a
   * member is missing or the child is found.
   * You may still get a nullptr with autocreate==true if the path is horribly defective, such as having ..'s that go above the root, or even more horribly if 'this' is null.
   * While ".." is specially treated as the parent of where you are in the path, "." is not implemented which means a child named "." will be looked for.
   * The ".." is honored anywhere in the path, although that is usually not sensible.
   * */
  Storable *findChild(TextKey path, bool autocreate = true); //# true as the default is legacy from the method this replaced.*/

  /** creates node if not present.*/
  Storable &child(TextKey childName);

  /** syntactic sugar for @see child(TextKey) */
  Storable &operator ()(TextKey name);
  //these give nth child
  Storable &operator [](unsigned ordinal);

  const Storable &operator [](unsigned ordinal) const;
  /** named version of operator [] const */
  const Storable &nth(unsigned ordinal) const;
  Storable &nth(unsigned ordinal);

private:
  /** find the index of a child node. @returns BadIndex if not a wad or not found in the wad.*/
  unsigned indexOf(const Storable &node) const;
public:
  /** add a new empty node */
  Storable &addChild(TextKey childName);//removed default, nameless nodes are rare, make them stand out.

  /** add a new node with content copied from existing one, created to clean up storedGroup entity copy, added altname to rename child since names are now const and sometimes must be unique (roles).*/
  Storable &createChild(const Storable &other, TextKey altname=nullptr);

  //combined presize and addChild to stifle trivial debug spew when adding a row to a table.
  Storable &addWad(unsigned qty, Storable::Type type = NotDefined, TextKey name = "");
  /** add a bunch of null-named children of the given type to this node. Probably not good to use outside of this class cloning an object.*/
  void presize(unsigned qty, Storable::Type type = NotDefined);

  /** remove a child of the wad, only makes sense for use with StoredGroup (or legacy cleanup) */
  bool remove(unsigned which);
  bool removeChild(Storable &node);
  bool removeChild(Cstr name);

  /** remove all children */
  void filicide(bool notify = false);
  /** remove from parent */
  void suicide(bool andDelete=false);//don't normally delete as someone is looking for the removal and they do the delete.
  /** @returns rootnode of this node, 'this' if 'this' is a root node. (i.e a root is its own parent using this method) */
  Storable &getRoot();
  /** force size of wad. */
  unsigned setSize(unsigned qty);
  /** find/create from an already parsed path. Honors '#3' notation for child [3]*/
  Storable *getChild(Sequence<Text> &progeny, bool autocreate);
private:
  Storable &finishCreatingChild(Storable &noob);
///////////////////////////////////
/** global/shared root, the 'slash' node for findChild */
  static Storable Slash;
public:
  /** access to @see Slash , the global/shared root*/
  static Storable &Groot(TextKey pathname);
  /** delete a node given an absolute pathname. @returns whether node was found and deleted */
  static bool Delete(TextKey pathname);
  /** find child given an absolute pathname. */
  static Storable *FindChild(TextKey pathname,bool autocreate);
  struct Mirror {
    virtual ~Mirror() = default;
    /** noob will have a parent */
    virtual void add(const Storable &noob) = 0;
    /** value changed */
    virtual void alter(const Storable &noob) = 0;
    /** implementor must copy over any members, noob will shortly be destructed. */
    virtual void remove(const Storable &noob) = 0;
  };
  static Mirror *remote;
}; // class Storable


struct StorageWalker {
  Storable *p;

  explicit StorageWalker(Storable *p) : p(p) {
  }

  explicit operator Storable *() {
    return p;
  }

  //todo: findChild variations.
  void push(Storable *child) {
    p = child;
  }

  void pop() {
    p = p->parent;
  }

  Storable *findChild(Text name, bool autocreate = true) {
    return p->findChild(name, autocreate);
  }

  bool empty() {
    return p == nullptr;
  }

  void pushChild(Text path, bool autocreate = true) {
    p = findChild(path, autocreate);
  }

  void setType(Storable::Type type) {
    p->setType(type);
  }
};

/** iterate over the children of given node (kinder is german  plural for child, like kindergarten) */
#define ForKinder(node) for(auto list(node.kinder()); list.hasNext(); )
#define ForKinderConstly(node) for(auto list(node.kinder()); list.hasNext(); )


/** auto creating iterator that provides for deleting the unscanned items.
 * todo:2 construction option to invoked done() in destructor. Handy for for loops, but not not always desired..*/
class StoredListReuser {
  Storable &node;
  unsigned wadding;
  unsigned pointer;
public:
  explicit StoredListReuser(Storable &node, unsigned wadding = 0);
  Storable &next();
  unsigned done();
};

#endif // STORABLE_H
