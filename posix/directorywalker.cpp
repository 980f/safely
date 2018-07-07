#include "directorywalker.h"


DirectoryWalker::DirectoryWalker(TextKey path):path(path)
{

}

void DirectoryWalker::flatread(std::function<void (Fildes &)> reader)
{
  this->action=reader;
  nftw(path,DirectoryWalker::ntftw);
}
