
/**
// Created by andyh on 1/31/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#include <bundler.h>
#include <counted.h>
////////////////////////// test
#include <iostream>
// #include <cstdint>
using std::cout;

class Demo:public Counted<Demo> , public Bundler<Demo>{
  // using Counted<Demo>;
public:
  void show(){
    cout<<'\t'<<myInstance;
  }
};

BundlerList(Demo);

Demo one;
Demo two;


// Counted(Demo);
template <> unsigned Counted<Demo>::numInstances=0;

int testGroupers_main(){
  cout<<"\ntwo statically built: ";
  cout<<Counted<Demo>::numInstances;
  one.show();
  two.show();
  cout<<"\nForEach:";
  Bundler<Demo>::ForEach(&Demo::show);
  Demo third;
  cout<<"\nLocal third";
  Demo::ForEach(&Demo::show);
  Demo *forth=new Demo;
  Demo *fifth=new Demo;
  cout<<"\nTwo dynamic";
  Demo::ForEach(&Demo::show);
  delete forth;
  cout<<"\ndelete first of those";
  Demo::ForEach(&Demo::show);
  cout<<"\nnumInstances: "<<one.numInstances;
  delete fifth;
  cout<<"\ndeleted other, leaving: "<<one.numInstances;
  Demo::ForEach(&Demo::show);
  //segv delete &one;
  return 0;
}
