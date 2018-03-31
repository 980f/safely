#include "hassettings.h"
#include "art.h"

unsigned numIDs(const char *list){
  unsigned num(0);
  while(*list++){
    ++num;
  }
  return num;
}

unsigned ordinalOfID(const char* list, ID id){
  unsigned i(0);
  while(char thing=*list++){
    if(thing==id){
      return i;
    } else {
      ++i;
    }
  }
  return BadIndex; //not found
}

HasSettings::HasSettings(const char *plist)://
  pMap(plist) {
}


void HasSettings::suppressField(ID fieldID) {
  if(fieldID=='*'){//added this to suppress useless emissions from qchardware.cpp
    pMap.queue.markAll(false);
    return;
  }
  BitReference bit = reportFor(fieldID);
  bit = 0;
}


//void HasSettings::startOOBdata(CharFormatter &response) {
//  if(response.removeTerminator()) {
//    response.next() = OOBmarker;
//  }
//}

bool HasSettings::blockCheck(HasSettings &desired, ID fieldID) {
  if(differs(desired)) {
    return post(fieldID);
  }
  return false;
}

////overload if you can't print args, such as for spectra.
//bool HasSettings::printField(ID fieldID, CharFormatter &response, bool master) {
//  MessageArgs;
//  response.printChar(fieldID);
//  if(getParam(fieldID, args)) {
//    response.printArgs(args, master);
//    response.addDigest();
//    response.addTerminator();
//    return true;
//  } else {
//    return false;
//  }
//}

bool HasSettings::post(char fieldID) {
  return pMap.post(fieldID);
}

bool HasSettings::differs(const HasSettings &other)const {
  for(Indexer<const char> fields(pMap.unitMap, pMap.quantity); fields.hasNext();) {
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
}

void HasSettings::copy(const HasSettings &other) {
  for(Indexer<const char> fields(pMap.unitMap, pMap.quantity); fields.hasNext();) {
    ID fieldID = fields.next();
    ArgBlock<MAXARGS> args;

    other.getParam(fieldID, args);
    args.freeze();//this freeze is why we reallocate the array on each loop.
    this->setParam(fieldID, args);
  }
}

bool HasSettings::differed(const HasSettings &other) {
  copy(other);
  return wasModified();
}

//bool HasSettings::parseArgstring(ArgSet &args, CharFormatter &p) {
//  while(args.hasNext()) {
//    double arg = p.parseDouble();
//    if(!isSignal(arg)) { //#isnormal inconveniently excludes zero.
//      args.next() = arg;
//    } else {
//      return false; //stop parse on invalid input
//    }
//    u8 ascii = p.next(0);
//    switch(ascii) {
//    case ',':
//      continue;
//    case 0:
//    case FrameBreak://checksum prefix
//      return true;
//    default:
//      return false;
//    }
//  }
//  return false;//should never get here. "more data than room for it"
//}

//////////////

//GroupMapper::GroupMapper(const char *unitMap):
//  units(unitMap) {
//  lastReporter = 0; //units exists, but peer is null at this time.
//}

//ParamKey GroupMapper::nextReport() {
//  OLM *scanner = lastReporter;
//  ParamKey nexrep;//note: debugger display the address of this variable instead of its value.
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
//  return nexrep;
//}

//void GroupMapper::markAll(bool send) {
//  for(OLM *scanner = units.peer; scanner; scanner = scanner->peer) {
//    scanner->queue.markAll(send);
//  }
//  //remove to see if this causes screaming writes: lastReporter = units.peer; //for predictable sequence
//}

////////////////

///** while it seems wasted effort to build a string that we are then going to pull apart, the textual frequency in the source code makes this sensible.*/
//bool Poster::postMessage(const char *twochar) {
//  return postKey(ParamKey(twochar));
//}
/////////////////


//////////////////////////
OLM &OLM::locate(ID code) {
  prefix = code;
  return *this;
}

OLM::OLM(const char *unitMap):
  unitMap(unitMap), //
  quantity(numIDs(unitMap)),//
  queue(quantity) {
  prefix = 0;
}

unsigned OLM::lookup(char ch, unsigned nemo) const {
  Index ordinal = ordinalOfID(unitMap, ch);
  return ordinal.isValid()? ordinal.raw:nemo;
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
ParamKey OLM::nextReport() {
  unsigned nexrep = queue.next();
  if(nexrep ==BadIndex) {
    return ParamKey();
  } else {
    return ParamKey(prefix, encode(nexrep));
  }
}

