  #include <iostream>
  #define TAB <<'\t'<<

#include "../../cortexm/lpc/baudsearch.h"

void toString(const BaudSearch &bs) {
#ifdef __linux__
    std::cout<<bs.baud TAB bs.divider TAB int(bs.div) TAB int(bs.mul);
    std::cout<<std::endl;
#endif
}

int main(int argc, char *argv[]){

  BaudSearch::desiredbaud=(argc>1)?atof(argv[1]):115200;
  BaudSearch::pclk=(argc>2)?atof(argv[2]):12000000;

  BaudSearch::snoop=&toString;

  BaudSearch::findBaudSettings();
  std::cout<<"baud" TAB "divider" TAB "div" TAB "mul" <<std::endl;
  toString(BaudSearch::nearest);

  return 0;
}
