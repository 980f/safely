#include "localonexit.h"


AutoFlag::AutoFlag(bool &toBeCleared) : ClearOnExit<bool>(toBeCleared){
  toBeCleared=true;
}


LocalOnExit::LocalOnExit(const LocalOnExit::SimpleFunction &deffered):deffered(deffered){

}

LocalOnExit::~LocalOnExit(){
  (*deffered)();
}
