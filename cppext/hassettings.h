#ifndef HASSETTINGS_H
#define HASSETTINGS_H

#include "buffer.h"
#include "chained.h" //can't use the better chain as we don't have a heap.
#include "charformatter.h"
//we will now coerce each field to be Settable:
#include "settable.h"

// message#extraneous data seperator
#define OOBmarker '#'

// message|checksum seperator
#define FrameBreak '|'

typedef char ID;

/** primitive human readable transport format
 *  simple enough to parse for a computer, simple enough to type for a human.
 *  limited to communicating sets of numbers, no text or tokens of any kind.
 *  Includes 'dirty' bit logic and a means for prioritizing sending dirty values without implementing a queue.
 *
 *  What should be tables assenbled by a linker are implemented with a linked list assembled by consrtuction of a static object.
 *  If we could get the linker tamed this all would be nicer to use.
 */

/** the key for our maps */
struct ParamKey {
  ID unit;
  ID field;
  /** defaulting either value gives you a false/useless one.*/
  ParamKey(u8 unit = 0,u8 field = 0) : unit(unit),field(field){
    //#nada
  }

  ParamKey(const char *twochar) : unit(*twochar++),field((*twochar)){
    //#nada
  }

  /**@return whether both unit and field are not ridiculous*/
  operator bool(){
    return unit!=0 &&field!=0;
  }

};

/** @returns strict less than compare.
 *  No known use, guessing a disgnostic tool wanted an alpha sort */
inline
bool operator<(const ParamKey& lhs, const ParamKey& rhs){
  if(lhs.unit == rhs.unit) {
    return lhs.field < rhs.field;
  } else {
    return lhs.unit < rhs.unit;
  }
}

/** @returns strict greater than compare.
 * No known use, guessing a disgnostic tool wanted an alpha sort */
inline
bool operator>(const ParamKey& lhs, const ParamKey& rhs){
  if(lhs.unit == rhs.unit) {
    return lhs.field > rhs.field;
  } else {
    return lhs.unit > rhs.unit;
  }
}

/** because bare const char *'s are deprecated for code review reasons, we typedef each class of use thereof:*/
typedef const ID * MnemonicSet;

//oldway needs to compile:
unsigned numIDS(MnemonicSet set);
unsigned ordinalOfID(MnemonicSet set);

#include "roundrobiner.h"
/** One Letter Mnemonic */
class OLM {
private:
  void operator =(int){
    //not sensible, these are used as dispatch table indices.
  }

public: //for use on GroupMapper by Settings grouper init //todo:3 friend?
  const MnemonicSet unitMap;
public: //for constructing reports 1:1 with queue entries on self as hasSettings
  unsigned quantity;//strlen unitMap
public:
  RoundRobiner queue;//these were 1:1 and so ... made it a member.
public: //todo:2 make read-only
  ID prefix;
public:
  OLM(MnemonicSet unitMap);
  OLM &locate(ID code);
  ~OLM();//uses permalloc
  /** report offset for given char*/
  unsigned lookup(ID ch,unsigned nemo = ~0) const;
  /** @see lookup*/
  unsigned operator()(ID asciiId) const;
  /** char for nth param*/
  ID encode(unsigned index) const;
  /** char for nth param*/
  ID operator[](unsigned index) const;

  bool post(ID fieldID){
    return queue.post(lookup(fieldID));
  }

  /** fieldID for next pending report, 0 for none*/
  ParamKey nextReport();
  bool &reportFor(ID fieldID) const {
    return queue.bit(lookup(fieldID));
  }

}; // class OLM

#include "changemonitored.h"

/** a thing which has sets of numerical values.*/
class HasSettings : public ChangeMonitored {
  friend class SettingsGrouper;
private:
  void operator =(int){
  }

protected:
  OLM pMap;
public:
  HasSettings(const char *plist);
  /** implementation selects item according to @param fieldID, then reads as many values as it needs.
   * @return true if the args and fieldID flag are copacetic*/
  virtual bool setParam(ID fieldID, ArgSet&args) = 0;
/** trigger a send, even if the data hasn't changed. */
  virtual void touch(ID fieldID);
  /** try to suppress a touch */
  void suppressField(ID fieldID);
  /** implementation should set args.next() equal to values in the same order that setParam expects them.
   * @return true if fieldID is known and there was room for the args.
   */
  virtual bool getParam(ID fieldID, ArgSet&args) const = 0;
  virtual Settable *field(ID fieldID);
  virtual void oobData(ID fieldId,CharFormatter &p);
  /** called by printReport when getParam returns false.
   * todo:M rework to call customReport which default:s to call getParam().
   */
  virtual CharScanner &customReport(ID fieldId) const;
  bool differs(const HasSettings &other) const;//compare
  bool differed(const HasSettings &other);//compare and copy
  void copy(const HasSettings &other);//non-recursive assignment

  /** flag deferred read */
  bool post(ID fieldID);
  int operator()(ID fieldID) const {
    return pMap(fieldID);
  }

  ID reportCodeFor(int which) const {
    return pMap.encode(which);
  }

  const char *postables(){
    return pMap.unitMap;
  }

  /** point to flag for deferred read*/
  bool &reportFor(ID fieldID) const {
    return pMap.reportFor(fieldID);
  }

  /** print values associated with field on given stringlike thing @param response */
  virtual bool printField(ID fieldID, CharFormatter&response,bool master);//#can't const as state machines may need to change when a message is actually sent.

  /** @return whether posting is attempted (useful for breakpoints), clears modified flags on both entities. */
  bool blockCheck(HasSettings &desired,ID fieldID);

  /** @return whether string appeared to be decent, after extracting comma separate numbers from it.*/
  static bool parseArgstring(ArgSet &args,CharFormatter &p);

  /** unterminate the buffer and mark it with the "more to come" flag*/
  static void startOOBdata(CharFormatter &);
}; // class HasSettings


struct CustomFormatter {
  virtual int printOn(CharFormatter &buf) const = 0;
};

#include "settable.h" //#this include is not needed locally but sooo many HasSettings instances include Settables this saved a bunch of rework.

class GroupMapper {
public:
//  OLM units;
//  GroupMapper(const char *unitMap);
//  OLM *lastReporter;
  ParamKey nextReport();
//  void link(OLM &pList){
//    units.append(&pList);
//  }

  void markAll(bool send = true);
}; // class GroupMapper

/**function pointer for sending implied information*/
class Poster {
public:
  bool postMessage(const char *twochar);
  virtual bool postKey(const ParamKey &parm) = 0;
};

#include "safestr.h"
/** finds a thing with settings given an ID code.
 *  Chained to link cores.*/
class SettingsGrouper : public Chained<SettingsGrouper>,public Poster {
protected:
  GroupMapper grouper;
public:
  SettingsGrouper(const char *unitList /*,bool *scoreboard,int scores*/);
  /**haven't yet managed to statically compile a linked list, which is theoretically possible with lots of fancy templating.*/
  void init();
  bool imLinkMaster;
  virtual HasSettings *unit4(ID asciiId,bool forRead = false) = 0;
  /** @return scoreboard index for given field's report*/
  bool &reportFor(const ParamKey &ID);
  /** @return scoreboard index for given field's report*/
  bool &bitFor(const char*twochar);
  /** request for deferred report*/
  bool postKey(const ParamKey &parm);

  /** next queued item's code.*/
  virtual ParamKey nextReport();
  /** @return null on success, else error message*/
  const char * stuff(const ParamKey &param, CharFormatter p);
  const char * stuff(const ParamKey &param, ArgSet &args);
  /** dump guts entirely, hopefully the stupid ones won't disturb the recipient.*/
  void sendAll(/*const char*except*/);
  /** build header then tail to @return getParams */
  bool printParam(CharFormatter&response, const ParamKey &Id,bool master); //exporting reporting from command responder to allow for deferred reports
  /** by default returns a pointer to a shared buffer, don't call printReport until the previous usage has been finished*/
  bool printReport(CharFormatter&response, const ParamKey &nexrep);
  /** get values for a named thing. @returns whether @param key was valid. This will not include any OutOfBand data. */
  bool getParam(const ParamKey &key,ArgSet &args);

}; // class SettingsGrouper

#endif // HASSETTINGS_H
