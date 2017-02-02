#include <iostream>

using namespace std;


#include <tuple>

//dummy writer, will pass a writer function into printer once it works well.
template<typename Any> void write(Any &c){
  cout<<c;
}

template<typename First,typename ... Args> auto select(int which,First first,Args ...args){
  if(which==0){
    return first;
  } else {
    return select(which-1,args...);
  }
}

template<typename ... Args>  void  Printf(const char *fmt, std::tuple<Args ...> &pack){
  char c;
  while((c=*fmt++)){
    if(c!='{'){
      write(c);
    }
    //we've seen an open curly
    unsigned argIndex=*fmt++-'0';//primitive get digit
    write( select<Args...>(argIndex,pack));
    if(*fmt++!='}'){
      cerr<<"formatting extensions NYI\n";
    }
  }
}


template<typename ... Args> static void  Printf(const char *fmt, const Args ... args){
  std::tuple<Args...>pack(args...);
  Printf(fmt,pack);
}


int main(int , char *[]){
  Printf("One {1} Two {2} Three {3}",1,2,3);
  return 0;
}
