#include <iostream>

using namespace std;

//dummy writer, will pass a writer function into printer once it works well.
template<typename Any> void write(Any &&c){
  cout<<c;
}

bool PrintItem(unsigned ){
  //must exist for compilation, should never get called.--unless arg is passed end of list.
  return false;//ran off end of arg list
}

template<typename First,typename ... Args> bool PrintItem(unsigned which, const First first, const Args ... args){
  if(which==0){
    write(first);
    return true;
  } else {
    return PrintItem(which-1,args...);
  }
}


template<typename ... Args> static void  Printf(const char *fmt, const Args ... args){
  char c;
  while((c=*fmt++)){
    if(c!='{'){
      write(c);
      continue;
    }
    //we've seen an open curly
    unsigned argIndex=*fmt++-'0';//primitive get digit
    if(!PrintItem(argIndex,args...)){
      write('{');
      write(char(argIndex+'0'));
      continue;
    }
    if(*fmt++!='}'){
      cerr<<"formatting extensions NYI\n";
    }
  }
}


int main(int , char *[]){
  Printf("One {0} Two {1} Three {2} and not {5}",1,2,"tree");
  cerr<<endl;
  return 0;
}
