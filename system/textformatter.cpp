#include "textformatter.h"

TextFormatter::TextFormatter(int presize) :
  Text(Zguard(presize)),  //allocate need data
  //nf has default init
  body(Cstr::violate(ptr),presize){ //wrap it (to keep track of allocation)

}

/** a zero init one of these computes the required size */
TextFormatter::TextFormatter() : TextFormatter(0){
  //#nada
}
