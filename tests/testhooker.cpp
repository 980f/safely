#include "/r/work/safely/cppext/hook.h"

#include <iostream>

void voidfunct(int ignore){
  std::cout<<"voidfunct->"<<ignore<<std::endl;
}

int intfunct(int ignore){
  return ignore;
}

int anotherfunct(int ignore){
  return ignore*17;
}

int main(int argc, char *argv[]){
  Hook<int> hook(voidfunct);

  Hooker<int,int> hook1(100, intfunct);

 // Hooker<void ,int> hook2(0, voidfunct);//fails due to trying to declare a void defaultReturn.
  hook(10);
  hook(15);
  std::cout<<"hook1(20): "<<hook1(20)<<std::endl;
  std::cout<<"hook1(25): "<<hook1(25)<<std::endl;

  hook1 = &anotherfunct;

  std::cout<<"hook1(20): "<<hook1(20)<<std::endl;
  std::cout<<"hook1(25): "<<hook1(25)<<std::endl;

  hook1=nullptr;
  std::cout<<"hook1(20): "<<hook1(20)<<std::endl;
  std::cout<<"hook1(25): "<<hook1(25)<<std::endl;

}
