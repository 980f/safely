//(C) 2017 Andrew Heilveil

#include "sigcuser.h"

using namespace sigc;

bool alwaysTrue(){
  return true;
}

/** @see invertSignal.*/
static void complement(bool value,BooleanSlot slot){
  slot(!value);
}

BooleanSlot invertSignal(BooleanSlot slot){
  return bind(&complement,slot);
}

//end complement signal

BooleanSlot assigner(bool &target){
  return sigc::bind(&assignValueTo<bool>, sigc::ref(target));
}

void onEdge(slot<bool> source,bool edge,SimpleSlot action){
  if(edge==source()) {
    action();
  }
}

///////////////////////////////////
Finally::Finally(const SimpleSlot &action) : action(action){
  //#nada
}

Finally::~Finally(){
  action();
}

//////////////////////////////////
ConnectionLocker::ConnectionLocker(sigc::connection &conn) : conn(conn){
  wasBlocked = conn.block();
}

ConnectionLocker::operator bool(){
  return !wasBlocked;
}

ConnectionLocker::~ConnectionLocker(){
  if(!wasBlocked) {//then we were the one that blocked it
    conn.unblock();
  }
}
