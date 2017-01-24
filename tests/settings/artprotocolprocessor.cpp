//#if ArtWithPosix       //won't be true for firmware, implies file systems and the like
////#include "sigcuser.h"
//#endif

#include "artprotocolprocessor.h"
#include "art.h" //where MaximumResponse is supposed to come from
#include "ctype.h"
#include "numberparser.h" //for convenience of no punctuation on sets.

/**
for the host allocate the maximum of:
//16000 for scope trace, 18432 for 2kX24 spectra 24576 for 4kX16 spectra, to spectra add some header space.
  8000X2 for diagnostic traces, 2k* decimal image of 16meg for 24 bit spectra.
*/

#ifndef MaximumResponse
#error "MaximumResponse is not declared in artprotocolprocessor.cpp, should be in art.h. Was 20000 at its greatest."
#endif

ArtProtocolProcessor::ArtProtocolProcessor(SettingsGrouper &sg, char *buf, int sizeofbuf):
  sg(sg), //
  framer(buf, sizeofbuf),
  successes(0),
  failures(0){
  //#nada
}

bool ArtProtocolProcessor::onReception(int incoming){
  return framer.onReception(incoming);
}

//only settable via debugger, for trapping some particular message:
static ID breakUnit=0;
static ID breakField=0;
static int breakCount=0;

/**skip past whitespace and return 1st non-white char*/
static u8 burnBlanks(CharScanner&p){
  u8 ascii = 0;

  while(p.hasNext()) {
    ascii = p.next();
    if(!isspace(ascii)) { //ignore whitespace and garbage NB:isblank only checks for space and tab
      return ascii;
    }
  }
  return 0;
} /* burnBlanks */

extern "C" void stackProbe(int dummy);

bool ArtProtocolProcessor::doLogic(){ //on any interrupt by any component
  bool handled = false;
  //todo:M create a separate MaxSend and MaxReceive value. This is too large in many cases.
  SafeStr <MaximumResponse> p;//this is a massive thing to be putting on the stack, on some systems.
  if(framer.getLine(p)) {
    bool fromMaster=false;
    bool csOk=true ; //--- p.checkDigest(true);

    ID unitId = burnBlanks(p);
    ID fieldID = 0;

    bool interactive = islower(unitId);
    if(!interactive&&!csOk){
      failed();
      return false;
    }
    unitId = toupper(unitId);
    HasSettings *unit = sg.unit4(unitId);
    if(!unit) {
      failed();
      return false;
    }
    fieldID = toupper(p.next(0));
    u8 command = p.next(0);

    switch(command) {
    default:
      if(!NumberParserPieces::startsNumber(command)) {
        failed();
        return false;
      }
      //else put that back ...
      p.unget();
      //#join assignment code.
    case '=': //receive assignment
      fromMaster=true;
    case ',':{ //receive echo or read request
      MessageArgs;//don't yet know how many are allowed by unit, unit's don't have to be settables.
      args.rewind(); //#COA
      if(HasSettings::parseArgstring(args,p)) { //execute
        args.freeze();
        if(breakUnit==unitId&& (!breakField||breakField==fieldID)){
          ++breakCount;//#ensure code doesn't optimize out, so that we can breakpoint here.
        }
#ifdef SIGCUSER_H
        packetWatcher(false,unitId,fieldID,args);
#endif
        handled=unit->setParam(fieldID, args);
        if(handled){
          ++successes;
          if(p.previous()==FrameBreak){//if we had a checksum
            p.parseInt();   //burn checksum
          }
          if(p.next()==OOBmarker){//then we have more data
            unit->oobData(fieldID,p);
          }
          if(fromMaster||interactive){
            unit->post(fieldID);
          }
        } else {
          failed();
        }
      }
    } break;
    case '/'://trigger reprocessing
      handled = true;
      unit->touch(fieldID);
      break;
    case 0: //name alone is a readback query
    case '?': //readback
      handled|=unit->post(fieldID);//set 'handled' to toggle LED even on read requests.
      break;
    } /* switch */
    return handled;
  } else { //we check for input on any interrupt, not just the host port interrupt. That way we can choose to only process part of the input buffer per system tick.
    return false;
  }
}

bool ArtProtocolProcessor::havaPacket() const {
  return framer.eols > 0;
}

void ArtProtocolProcessor::failed(){
  ++failures;
}
