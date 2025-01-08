#include "hook.h"

void voidfunct(int ignore){
}

int intfunct(int ignore){
  return 0;
}

int main(int argc, char *argv[]){
  Hook<int> hook(voidfunct);

  Hooker<int,int> hook1(15, intfunct);

  Hooker<void ,int> hook2(0, voidfunct);//fails due to trying to declare a void defaultReturn.

}
