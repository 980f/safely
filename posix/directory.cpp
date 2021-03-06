#include "directory.h"


Directory::Directory(const char *base):PosixWrapper ("Directory"),
  base(base),
  dirp(nullptr){
  dirp=opendir(base);
  if(!dirp){
    setFailed(true);
  }
}

dirent *Directory::next(){
  return readdir(dirp);
}
