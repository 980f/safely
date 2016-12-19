#include "cheapTricks.h"
#include "minimath.h"

bool isPresent(const char *flags, char flag){
  int badStringLimiter = 26; //in case string pointer is garbage we don't want to read all of ram

  if(flags) {
    char probe;
    while((probe = *flags++) && badStringLimiter-- > 0) {
      if(flag == probe) {
        return true;
      }
    }
  }
  return false;
} /* isPresent */

bool changed(double&target, double newvalue,int bits){
  if(!nearly(target,newvalue,bits)){
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

AutoFlag::AutoFlag(bool &toBeCleared):ClearOnExit<bool>(toBeCleared,true){
  //#nada
}
