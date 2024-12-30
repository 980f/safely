#include "cppexttester.h"
#include "stdio.h"

CppExtTester::CppExtTester()
{
}


#include "twiddler.h"
void CppExtTester::TwiddleTest(int which)
{
  switch(which){
    case 0: {
      Twiddler test(4,5);
      if(test.getRatio()!= 4.0/5){
        printf("Twiddler bad ratio 4/5");
      }
      }break;
  }
}


int main(int argc, char *argv[]){
  CppExtTester tester;
  tester.TwiddleTest(0);
  printf("\nTests completed.\n");
}
