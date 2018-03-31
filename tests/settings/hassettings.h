#ifndef HASSETTINGS_H
#define HASSETTINGS_H

#include "buffer.h"
#include "charformatter.h"

typedef char ID;
//not using strlen because the firmware linker catches the proverbial whale (printf)
unsigned numIDs(const char *list);
unsigned ordinalOfID(const char* list, ID id);

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
  ParamKey (char unit=0,char field=0):unit(unit),field(field){
    //#nada
  }

  ParamKey (const char *twochar):unit(*twochar++),field((*twochar)){
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
typedef const char * MnemonicSet;

#include "roundrobiner.h"
/** One Letter Mnemonic */
class OLM {
private:
  void operator =(int)=delete;   //not sensible, these are used as dispatch table indices.
public: //for use on GroupMapper by Settings grouper init //todo:3 friend?
  const MnemonicSet unitMap;
public: //for constructing reports 1:1 with queue entries on self as hasSettings
  unsigned quantity;//strlen unitMap
public:
  RoundRobiner queue;//these were 1:1 and so ... made it a member.
public:
  //cached tag for generating ParamKey's expeditiously.
  ID prefix;
public:
  OLM(MnemonicSet unitMap);
  OLM &locate(ID code);
  virtual ~OLM()=default;
  /** report offset for given char*/
  unsigned lookup(ID ch,unsigned nemo = BadIndex) const;
  /** @see lookup*/
  unsigned operator()(ID asciiId) const;
  /** char for nth param*/
  ID encode(unsigned index)const;
  /** char for nth param*/
  ID operator[](unsigned index)const;

  bool post(ID fieldID){
    return queue.post(lookup(fieldID));
  }

  /** fieldID for next pending report, 0 for none*/
  ParamKey nextReport();
  BitReference reportFor(ID fieldID) {
    return queue.bit(lookup(fieldID));
  }
};

#include "changemonitored.h"

/** a thing which has sets of numerical values.*/
class HasSettings :public ChangeMonitored {
  void operator =(int)=delete;
protected:
  OLM pMap;
public:
  HasSettings(const char *plist);
  virtual ~HasSettings()=default;
  /** implementation selects item according to @param fieldID, then reads as many values as it needs.
    * @return true if the args and fieldID flag are copacetic*/
  virtual bool setParam(ID fieldID, ArgSet&args) = 0;

  /** implementation should set args.next() equal to values in the same order that setParam expects them.
    * @return true if fieldID is known and there was room for the args.
    */
  virtual bool getParam(ID fieldID, ArgSet&args) const = 0;

  bool differs(const HasSettings &other)const;//compare
  bool differed(const HasSettings &other);//compare and copy
  /** @return whether posting is attempted (useful for breakpoints), clears modified flags on both entities. */
  bool blockCheck(HasSettings &desired,ID fieldID);
  void copy(const HasSettings &other);//non-recursive assignment

  /** try to suppress a touch */
  void suppressField(ID fieldID);
  /** flag deferred read */
  bool post(ID fieldID);
  unsigned operator()(ID fieldID)const{
    return pMap(fieldID);
  }

  ID reportCodeFor(unsigned which) const {
    return pMap.encode(which);
  }

  const char *postables(){
    return pMap.unitMap;
  }

  /** point to flag for deferred read*/
  BitReference reportFor(ID fieldID) {
    return pMap.reportFor(fieldID);
  }

};



#endif // HASSETTINGS_H
