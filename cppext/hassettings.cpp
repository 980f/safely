#include "safely.h"
#include "hassettings.h"
#include "string.h" //strlen  strchr
#include "permalloc.h"

#include "art.h" //protocol parameters
//legacy:
//oldway needs to compile:
unsigned numIDS (MnemonicSet set){
  return strlen(set);
}

HasSettings::HasSettings(const char *plist) ://
  pMap(plist){
}

//void HasSettings::touch(ID /*ignored*/){
//  //in the absence of override touch the whole unit.
//  also(true);
//}

void HasSettings::suppressField(ID fieldID){
  if(fieldID=='*') {//added this to suppress useless emissions from qchardware.cpp
    pMap.queue.markAll(false);
    return;
  }
  BitReference bit = reportFor(fieldID);
  bit = 0;
}

Settable *HasSettings::field(ID fieldID){
  return nullptr;//we'll find out why this has gone missing
}


bool HasSettings::blockCheck(HasSettings &desired, ID fieldID){
  if(differs(desired)) {
    return post(fieldID);
  }
  return false;
}

////overload if you can't print args, such as for spectra.
//bool HasSettings::printField(ID fieldID, CharFormatter &response, bool master){
//  ArgBlock<15> args;
//  response.printChar(fieldID);
//  if(getParam(fieldID, args)) {
//    response.printArgs(args, master);
//    //-- remove due to lack of generallity, need to implement locally
//    response.addDigest();
//    response.addTerminator();
//    return true;
//  } else {
//    return false;
//  }
//} // HasSettings::printField

bool HasSettings::post(char fieldID){
  return pMap.post(fieldID);
}

bool HasSettings::differs(const HasSettings &other) const {
  for(Indexer<const char> fields(pMap.unitMap, pMap.quantity); fields.hasNext(); ) {
    ID fieldID = fields.next();
    ArgBlock<MAXARGS> args;
    ArgBlock<MAXARGS> args2;
    other.getParam(fieldID, args);
    this->getParam(fieldID, args2);
    if(!args2.equals(args)) {
      return true;
    }
  }
  return false;
} // HasSettings::differs

void HasSettings::copy(const HasSettings &other){
  for(Indexer<const char> fields(pMap.unitMap, pMap.quantity); fields.hasNext(); ) {
    ID fieldID = fields.next();
    ArgBlock<MAXARGS> args;
    other.getParam(fieldID, args);
    args.freeze();//this freeze is why we reallocate the array on each loop.
    this->setParam(fieldID, args);
  }
}

bool HasSettings::differed(const HasSettings &other){
  copy(other);
  return wasModified();
}

////////////

//GroupMapper::GroupMapper(const char *unitMap) :
//  units(unitMap){
//  lastReporter = 0; //units exists, but peer is null at this time.
//}

ParamKey GroupMapper::nextReport(){
//  OLM *scanner = lastReporter;
  ParamKey nexrep;//note: debugger display the address of this variable instead of its value.
//  while(!(nexrep = scanner->nextReport())) {
//    scanner = scanner->peer;
//    if(!scanner) { //maydo: make a circle when we init
//      scanner = units.peer;
//    }
//    if(scanner == lastReporter) {
//      break;
//    }
//  }
//  if(nexrep) {
//    lastReporter = scanner;
//  }
  return nexrep;
} // GroupMapper::nextReport

void GroupMapper::markAll(bool send){
//  for(OLM *scanner = units.peer; scanner; scanner = scanner->peer) {
//    scanner->queue.markAll(send);
//  }
//  //removed to see if fixes 'screaming writes' //todo:1 research whether we should revert this: lastReporter = units.peer; //for predictable sequence
}

////////////

/** while it seems wasted effort to build a string that we are then going to pull apart, the textual frequency in the source code makes this sensible.*/
bool Poster::postMessage(const char *twochar){
  return postKey(ParamKey(twochar));
}

CharScanner &HasSettings::customReport(ID /*ignored*/) const {
  return CharScanner::Null;
}

//////////////////////////
OLM &OLM::locate(ID code){
  prefix = code;
  return *this;
}

OLM::OLM(MnemonicSet unitMap) :
  unitMap(unitMap), //
  quantity(strlen(unitMap)),//
  queue(quantity){
  prefix = 0;
}


unsigned OLM::lookup(char ch, unsigned nemo) const {
  const char *p = strchr(unitMap, ch);
  return p ? p - unitMap : nemo;
}

unsigned OLM::operator()(ID asciiId) const {
  return lookup(asciiId);
}

char OLM::encode(unsigned index) const {
  return index < quantity ? unitMap[index] : 0;
}

char OLM::operator[](unsigned index) const {
  return encode(index);
}

/** fieldID for next pending report, 0 for none*/
ParamKey OLM::nextReport(){
  unsigned nexrep = queue.next();
  if(nexrep!=BadIndex) {
    return ParamKey(prefix, encode(nexrep));
  } else {
    return ParamKey();
  }
}

///////////////////
