#ifndef SYSTEMTESTER_H
#define SYSTEMTESTER_H

#include "storable.h"
#include "stdio.h"

class SystemTester {

public:
  SystemTester();
  void testStorable(){
    Storable node("safely system library");
    node.child("a chld").setNumber(15.678);
    printf("Stored: %s",node.image().c_str());
  }

}; // class SystemTester


//int main(int argc,char *argv[]){
//  SystemTester tester;

//}

#endif // SYSTEMTESTER_H
