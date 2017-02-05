#include <iostream>
using namespace std;

#include "streamprintf.h"
int main(int, char *[]){
  StreamPrintf pf(cout);
  pf.Printf("One {0} Two {1:10} Three {2} and not {5}",1,2.3,"tree");
  cerr << endl;
  return 0;
}
