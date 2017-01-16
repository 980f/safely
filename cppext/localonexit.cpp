#include "localonexit.h"


AutoFlag::AutoFlag(bool &toBeCleared) : ClearOnExit<bool>(toBeCleared,true){
  //#nada
}


LocalOnExit::LocalOnExit(const LocalOnExit::SimpleFunction &deffered):deffered(deffered){

}

LocalOnExit::~LocalOnExit(){
  (*deffered)();
}
