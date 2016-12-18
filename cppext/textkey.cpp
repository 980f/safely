#include "textkey.h"
#include "stdlib.h"
#include "string.h"

bool nonTrivial(const TextKey t){
  return t != nullptr && *t != 0;
}


bool same(const TextKey a, const TextKey b){
  if(a == b) {
    return true;
  }
  if(a == nullptr) {
    return *b == 0;
  }
  if(b == nullptr) {
    return *a == 0;
  }
  if(0 == strcmp(a, b)) {
    return true;
  } else {
    return false;
  }
} // same

double toDouble(const TextKey rawText, bool *impure){
  char *end(nullptr);  //setting value for debug purposes
  /*todo:1 set locale to one that doesn't have triplet separators */
  double d = strtod(rawText, &end);

  if(impure) {
    *impure = nonTrivial(end);
  }
  return d;
}
