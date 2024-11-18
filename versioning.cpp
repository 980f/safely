#include "versioning.h"  //which file just mentions this function and seems to repeatedly get dropped from the repo.
/** even after moving from svn to git we desired a sequential build number, for 'reasons'. So, that build script actually looks for tags formatted specially finding the largest numerical value embedded in them, adds one, tags the repo with that and then builds the installation artifact file.*/
double svn(){
  return
    #include "svnrevision.txt"
  ;
}
