#pragma once
/**
// Created by andyh on 1/31/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
derive your class from Counted<yourclass> and this uniquely numbers them keeps track of maximum existing instances
*/


template<class Grouped> class  Counted {
public:
  static unsigned numInstances;
  unsigned myInstance;

  Counted(){
    myInstance = numInstances++;
  }

  ~Counted(){
    //the following hiding of instances created and destroyed before any others can be might be gratuitous. It is a tweak to hide locally created and deleted entities. This class actually is not very useful if you mix static and dynamic instantiations.
    if(myInstance == numInstances-1){
      --numInstances;
    }
    myInstance = ~0;//in case of use after free.
  }

};
//the following macro changed between c++17 and c++23: we might have to do some ifdef'ing on c++ versions
#define Counted(Grouped) template <> unsigned Grouped::Counted<Grouped>::numInstances=0
