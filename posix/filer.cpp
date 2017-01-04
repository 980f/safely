#include "filer.h"

#include "sys/stat.h"
#include "sys/types.h" //mkdir
#include "errno.h"
#include <cstdlib>

#include "logger.h"
#include "filename.h"


Filer::Filer(){
  buffer = 0;
}

Filer::~Filer(){
  delete[]  buffer;
}

bool Filer::mkDirDashP(const char *path, bool itsparent){
  FileName dirpath(path);

  if(itsparent) {
    dirpath.dirname();
  }
  if(dirpath.empty()) { //empty is not the same as '/', and even if it were we won't allow ourselves to create '/', ain't meaningful AFAIK
    return false;
  }
  PathParser::Brackets wrapwith(true,true);
  Text normalized=dirpath.pack(wrapwith);
  if(mkdir(normalized.c_str(), 0777) == 0) {
    return true;
  }
  if(errno == EEXIST) { //failed because directory already existed.
    return true; //wtf did posix make it an error to try to make a directory that already exists!?
  }
  if(errno == ENOENT) { //file doesn't exist
    if(mkDirDashP(normalized.c_str(), true)) {
      int wtfn = mkdir(normalized.c_str(), 0777); // //now that parent exists we try again
      return wtfn == 0;
    }
    return false;
  } else {
    return false;
  }
} // mkDirDashP

bool Filer::openFile(const char *fname, int o_flags, bool makeDirs){
  if(fd.open(fname, o_flags)) {
    return true; //worked first try
  }
  if(fd.errornumber == ENOENT) { //if file or directory don't exist
    if(makeDirs) {
      if(mkDirDashP(fname)) {
        return openFile(fname, o_flags, false); //last try
      } else {
        return false; //simple could not manage to do it.
      }
    } else {
      return false; //not given permission to make directory at need
    }
  } else {
    return false; //not an error that we can deal with
  }
} // openFile

FILE *Filer::getfp(const char *fargs){
  return fd.getfp(fargs);
}

int Filer::close(){
  return fd.close();
}

ByteScanner Filer::contents(){
  return ByteScanner(buffer, size);
}

bool Filer::readall(int maxalloc){
  if(fd.isOpen()) {
    struct stat info;
    int arf = fstat(fd, &info);
    if(arf == 0) {
      size = info.st_size;
      if(size > maxalloc) {
        size = maxalloc;
      }
      buffer = new unsigned char[size];  //DEL@ destructor
      ByteScanner scanner(buffer, size);
      int read = fd.read(scanner);
      if(read != size) {
        logmsg("funky read in Filer::readall:%d of %d", read, size);
        read = size;
      }
      return true;
    }
  }
  return failure();
} /* readall */

bool Filer::cp(const char *src, const char *target, bool dashf, bool dashr){
  FileName from(src);
  FileName to(target);

  SegmentedName command;
  command.prefix("cp");
  if(dashf && dashr) {
    command.suffix("-rf");
  } else if(dashf) {
    command.suffix("-f");
  } else if(dashr) {
    command.suffix("-r");
  }
  command.suffix(from.pack());
  command.suffix(to.pack());

  if(int arf = system(PathParser::pack(command,' '))) {//posixwrap
    arf = system("sync");
    return true;
  } else {
    return false;
  }
} // cp

/** the implementation below strips leading and trailing slashes, we can't remove absolutely named files here. */
int Filer::rm(const char *name, bool dashf, bool dashr){
  if(nonTrivial(name)) {
    FileName full(name);
    if(dashf || dashr) { //must use Shell
      SegmentedName command;
      command.suffix("rm");
      if(dashf) {
        command.suffix("-f");
      }
      if(dashr) {
        command.suffix("-r");
      }
      command.transfer(full);
      return system(PathParser::pack(command,' '));
    } else { //do it the simple way
      return remove(full.pack());//running it through FileName purifies the string by our rules, which might be nicer than the OS.
    }
  } else { //trivial name
    return 0; //success! we removed nothing!
  }
} // rm

bool Filer::exists(const char *name){
  if(isTrivial(name)) {
    return false; //pathological
  }
  struct stat st;
  if(0 == ::stat(name, &st)) {
    return true;
  } else {
    return false;
  }
} // exists

int Filer::mv(const char *src, const char *target){
  return rename(src, target); //todo:1 capture errno etc stuff.
}
