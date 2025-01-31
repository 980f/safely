
/**
// Created by andyh on 1/31/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#include "fibonaccigenerator.h"
#include "cliscanner.h"
#include <iostream>
void testFibongenerator(int argc, char *argv[]) {
  CliScanner argparser(argc, argv);
  unsigned even=0;
  unsigned odd=1;
  if (argparser.stillHas(2)) {
    argparser>>even;
    argparser>>odd;
  }
  FibonacciGenerator<unsigned,true> fibber(even,odd);
  std::cout<<"Generating fibonacci numbers starting with \t"<<even<<"\t"<<odd<<std::endl;
  do {
    std::cout<<"\t"<<fibber();
  } while(fibber.ordinal()<200);//todo: add cli arg for limit,
}
