#include "directorywalker.h"
#include "fcntlflags.h"


std::function<void(Fildes &,FTW &ftw)> DirectoryWalker ::faction;

int DirectoryWalker::readerthunk(const char *path, const struct stat *stat, int flags, FTW *ftw){
  dbg("nftw:%d.%d(%X and %X) on %s",ftw->base,ftw->level,stat->st_mode,flags,path);
  Fildes fd("Walker");
  //S_ISREG  S_ISDIR  S_ISCHR S_ISBLK S_ISFIFO
  if(S_ISREG(stat->st_mode)) {
    fd.open(path,O_RDONLY);
    faction(fd,*ftw);
  }
  return FTW_CONTINUE;
}

DirectoryWalker::DirectoryWalker(TextKey path) : path(path){

}

void DirectoryWalker::flatread(std::function<void (Fildes &)> reader){
  flatread([reader](Fildes &fd,FTW &){reader(fd);});
}


void DirectoryWalker::flatread(std::function<void(Fildes &,FTW &)> reader){
  faction= reader;
  nftw(path,DirectoryWalker::readerthunk,0,0);
  faction=nullptr;//in case it binds a deletable param
}
