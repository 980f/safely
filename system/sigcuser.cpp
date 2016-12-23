/**************************************************************************
** Copyright (C) 2012 Rigaku (Applied Rigaku Technologies)
** created: 1/24/2012
**************************************************************************/

#include "sigcuser.h"
//#include <gtkmm.h> //signal timeout

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

//void doSoon(SimpleSlot slot, int howSoon, int howurgently){
//  Glib::signal_timeout().connect_once(slot, howSoon,Glib::PRIORITY_DEFAULT_IDLE - howurgently);
//}

//SimpleSlot eventually(SimpleSlot toDefer){
//  return bind(&doSoon,toDefer,0,1);
//}

/////////////////////////////////////

//RunOnceSlot::RunOnceSlot(SimpleSlot action) : action(action){
//  //#nada
//}

//SimpleSlot RunOnceSlot::makeInstance(SimpleSlot action){
//  RunOnceSlot &dou(*new RunOnceSlot(action));
//  return mem_fun(dou,&RunOnceSlot::run);
//}

//void RunOnceSlot::run(){
//  DeleteOnReturn<RunOnceSlot> dor(this);//in case of survivable exceptions
//  action();
//}

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
