#include "safely.h"
#include "hassettings.h"
#include "string.h" //strlen  strchr
#include "permalloc.h"

//legacy:
//oldway needs to compile:
unsigned numIDS (MnemonicSet set){
  return  strlen(set);
}


HasSettings::HasSettings(const char *plist) ://
  pMap(plist){
}

void HasSettings::touch(ID /*ignored*/){
  //in the absence of override touch the whole unit.
  also(true);
}

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

void HasSettings::oobData(ID /*fieldId*/, CharFormatter & /*p*/){
  //#ignore.
}

void HasSettings::startOOBdata(CharFormatter &response){
  if(response.removeTerminator()) {
    response.next() = OOBmarker;
  }
}

bool HasSettings::blockCheck(HasSettings &desired, ID fieldID){
  if(differs(desired)) {
    return post(fieldID);
  }
  return false;
}

//overload if you can't print args, such as for spectra.
bool HasSettings::printField(ID fieldID, CharFormatter &response, bool master){
  MessageArgs;
  response.printChar(fieldID);
  if(getParam(fieldID, args)) {
    response.printArgs(args, master);
    //-- remove due to lack of generallity, need to implement locally response.addDigest();
    response.addTerminator();
    return true;
  } else {
    return false;
  }
} // HasSettings::printField

bool HasSettings::post(char fieldID){
  return pMap.post(fieldID);
}

bool HasSettings::differs(const HasSettings &other) const {
  for(Indexer<const char> fields(pMap.unitMap, pMap.quantity); fields.hasNext(); ) {
    ID fieldID = fields.next();
    MessageArgs;
    MessageArgs2;
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
    MessageArgs;
    other.getParam(fieldID, args);
    args.freeze();//this freeze is why we reallocate the array on each loop.
    this->setParam(fieldID, args);
  }
}

bool HasSettings::differed(const HasSettings &other){
  copy(other);
  return wasModified();
}

bool HasSettings::parseArgstring(ArgSet &args, CharFormatter &p){
  while(args.hasNext()) {
    double arg = p.parseDouble();
    if(!isSignal(arg)) { //#isnormal inconveniently excludes zero.
      args.next() = arg;
    } else {
      return false; //stop parse on invalid input
    }
    char ascii = p.next(0);
    switch(ascii) {
    case ',':
      continue;
    case 0:
    case FrameBreak://checksum prefix
      return true;
    default:
      return false;
    }
  }
  return false;//should never get here. "more data than room for it"
} // HasSettings::parseArgstring

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

///////////////

SettingsGrouper::SettingsGrouper(const char *unitList)
//  :grouper(unitList)
{
  //#subordinate units may not yet be constructed so this is too soon to init the lookup mechanism.
  imLinkMaster = false;
}

void SettingsGrouper::init(){
//  for(const char *scanner = grouper.units.unitMap; *scanner; ++scanner) {
//    HasSettings *unit = unit4(ID(*scanner));
//    if(!unit) {
//      continue;//a command group is not yet implemented
//    }
//    grouper.link((unit->pMap.locate(*scanner)));
//  }
//  grouper.lastReporter = grouper.units.peer;
//  if(peer) {
//    peer->init();
//    grouper.link(*(peer->grouper.units.peer));//1st child of next group
//  }
} // SettingsGrouper::init

BitReference SettingsGrouper::reportFor(const ParamKey &ID){

  HasSettings *unit = unit4(ID.unit);
  if(unit) {
    return unit->reportFor(ID.field);
  }
  return BitReference(0U,0);//null pointer, should blow emphatically rather than vaguely
}

/** @return scoreboard index for given field's report*/
BitReference SettingsGrouper::bitFor(const char *twochar){
  return reportFor(ParamKey(twochar));
}

bool SettingsGrouper::printParam(CharFormatter &response, const ParamKey &Id, bool master){
  HasSettings *unit = unit4(Id.unit, true);
  if(unit) {
    //maydo: a transactional buffer around response starting here.
    response.printChar(Id.unit);
    return unit->printField(Id.field, response, master);
  } else {
    return false;
  }
}

bool SettingsGrouper::printReport(CharFormatter &response, const ParamKey &key){
  return printParam(response, key, imLinkMaster);
}

bool SettingsGrouper::getParam(const ParamKey &key,ArgSet &args) {
  if(HasSettings *unit = unit4(key.unit, true)) {
    return unit->getParam(key.field, args);
  } else {
    return false;
  }
}

bool SettingsGrouper::postKey(const ParamKey &parm){
  HasSettings *unit = unit4(parm.unit);
  if(unit) {
    return unit->post(parm.field);
  }
  return false;
}

ParamKey SettingsGrouper::nextReport(){
  return grouper.nextReport();
}

void SettingsGrouper::sendAll(/*todo: const char*except*/){
  grouper.markAll();
}

const char * SettingsGrouper::stuff(const ParamKey &param, CharFormatter p){
  HasSettings *unit = unit4(param.unit,false);
  if(unit) {
    MessageArgs;
    if(HasSettings::parseArgstring(args,p)) { //execute
      ArgSet clipped(args);
      if(unit->setParam(param.field, clipped)) {
        return 0;
      } else {
        return "bad fieldID";
      }
    } else {
      return "bad value string";
    }
  } else {
    return "bad unitID";
  }
} // SettingsGrouper::stuff

const char * SettingsGrouper::stuff(const ParamKey &param, ArgSet &args){
  HasSettings *unit = unit4(param.unit,false);
  if(unit) {
    ArgSet clipped(args);
    if(unit->setParam(param.field, clipped)) {
      return 0;
    } else {
      return "bad fieldID";
    }
  } else {
    return "bad unitID";
  }
} // SettingsGrouper::stuff

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

OLM::~OLM(){
//  StaticFree(&queue);
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
  int nexrep = queue.next();
  if(nexrep < 0) {
    return ParamKey();
  } else {
    return ParamKey(prefix, encode(nexrep));
  }
}

///////////////////
