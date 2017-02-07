#include <iostream>
using namespace std;

#include "streamprintf.h"

int main(int argc, char *argv[]){
  StreamPrintf pf(cout);
  unsigned which=(argc>1)?atoi(argv[1]):BadIndex;
  if(isValid(which++)){
    if(which<argc){
      pf.Printf(argv[which],1,2.3,"tree",-4.1);
    } else {
      cerr<<"Bad test selection index"<<endl;
    }
  } else {
    for(which=argc;which-->2;){//argv0 is command, argv.1 is which index
      pf.Printf(argv[which],1,2.3,"tree",-4.1);
    }
  }
  cerr << endl;
  return 0;
}
