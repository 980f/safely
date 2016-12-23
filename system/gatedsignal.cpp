#include "gatedsignal.h"
#include "logger.h"
#include "cheapTricks.h"

GatedSignal::GatedSignal() :
  gateCounter(0),
  doEmit(false){
}

SimpleSignal::iterator GatedSignal::connect(const SimpleSlot &_slot){
  return mySignal.connect(_slot);
}

void GatedSignal::send(){
  if(gateCounter == 0) {
    mySignal();
  } else {
    doEmit = true;
  }
}

void GatedSignal::gate(){
  gateCounter++;
}

void GatedSignal::ungate(){
  if(--gateCounter == 0) {
    if(flagged(doEmit)) {
      mySignal();
    }
  } else if(gateCounter < 0) {
    dbg("mismatched ungate");
  }
}

SimpleSlot GatedSignal::propagator(){
  return MyHandler(GatedSignal::send);
}

/////////////////
GatedSignal::Freezer::Freezer(GatedSignal &gs) : gs(gs){
  gs.gate();
}

GatedSignal::Freezer::~Freezer(){
  gs.ungate();
}