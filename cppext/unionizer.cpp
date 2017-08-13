#include "unionizer.h"
#include "logger.h"
#include "eztypes.h"
#include <cinttypes>

Unionizer::Unionizer(){
 storage.dee=0;
}

void Unionizer::testUsage(){
  Unionizer u;
  u.storage.eye=42;
  int &iref=u.as<int>();
  dbg("int 42 as int: %d or %d ",u.as<int>(),iref);
  dbg("As double: %g",u.as<double>());
  double &dref=u.as<double>();
  dref=42;
  dbg("double 42 as int: %d , unt64: 0x%jx ",iref,u.as<uintmax_t>());
  dbg("As double: %g",u.as<double>());
  bool &bref=u;
  bref=true;
  dbg("True as unsigned: %u",u.as<unsigned>());
  uintmax_t &um=u;
  for(int i=0;i<10;++i){
    dref=i;
    dbg("%d: %g %jx",i,dref,um);
  }

}
