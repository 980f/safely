#include "multilock.h"

MultiLock::MultiLock():locked(0){
}

bool MultiLock::lock(){
  if(!locked++){//#leave expanded for debug
    return true;
  } else {
    return false;
  }
}

bool MultiLock::unlock(){
  if(locked){//guard against prophylactic unlocks
    if(!--locked){
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MultiLock::setlock(bool lockit){
  lockit?lock():unlock();
}

bool MultiLock::isLocked()const{
  return locked!=0;
}


//////////////////////////
bool MultiLocker::lock(){
  if(!locked++){
    return resource.lock();
  } else {
    return false;
  }
}

bool MultiLocker::unlock(){
  if(locked){//guard against prophylactic unlocks
    if(!--locked){
      return resource.unlock();
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void MultiLocker::setlock(bool lockit){
  lockit?lock():unlock();
}

void MultiLocker::operator =(bool lockit){
  setlock(lockit);
}

bool MultiLocker::isLocked() const{
  return locked!=0;
}

MultiLocker::MultiLocker(MultiLock &lock):resource(lock){
  //#nada
}

MultiLocker::~MultiLocker(){
  if(locked){
    resource.unlock();
  }
}

bool MultiLocker::resourceLocked()const{
  return resource.isLocked();
}


AutoMultiLock::~AutoMultiLock(){
  resource.unlock();
}

AutoMultiLock::AutoMultiLock(MultiLock &lock):resource(lock){
  resource.lock();
}
