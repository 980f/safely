#include "filer.h"
#include "sys/stat.h"
#include "sys/types.h" //mkdir
#include "filename.h"
#include "errno.h"

#include "logger.h"
#include "string.h"

//#include "shell.h"
#include "safestr.h"

#include "textkey.h"

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
  if(mkdir(dirpath.c_str(), 0777) == 0) {
    return true;
  }
  if(errno == EEXIST) { //failed because directory already existed.
    return true; //wtf did posix make it an error to try to make a directory that already exists!?
  }
  if(errno == ENOENT) { //file doesn't exist
    if(mkDirDashP(dirpath.c_str(), true)) {
      int wtfn = mkdir(dirpath.c_str(), 0777); // //now that parent exists we try again
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


  SafeStr<512> command;
  command.cat("cp ");
  if(dashf && dashr) {
    command.cat("-rf ");
  } else if(dashf) {
    command.cat("-f ");
  } else if(dashr) {
    command.cat("-r ");
  }
  command.cat(from.c_str());
  command.cat(" ");
  command.cat(to.c_str());
  int arf = system(command.asciiz());
  if(arf) {//todo:1 this 'if' statement seems to be inverted, went through false branch after a successful copy.
    arf = system("sync");
    return true;
  } else {
    return false;
  }
#endif // if UseShellClass
} // cp

int Filer::rm(const char *name, bool dashf, bool dashr){
  if(nonTrivial(name)) {
    FileName full(name);
    if(dashf || dashr) { //must use Shell
#if UseShellClass
      std::vector<std::string> argv;
      argv.push_back("rm");
      if(dashf) {
        argv.push_back("-f");
      }
      if(dashr) {
        argv.push_back("-r");
      }
      argv.push_back("--");
      argv.push_back(full.c_str());
      return Shell::run(argv);

#else // if UseShellClass
      SafeStr<512> command;
      command.cat("rm ");
      if(dashf) {
        command.cat(" -f ");
      }
      if(dashr) {
        command.cat(" -r ");
      }
      command.cat(name);
      return system(command.asciiz());

#endif // if UseShellClass
    } else { //do it the simple way
      return remove(full.c_str());
    }
  } else { //trivial name
    return 0; //success! we removed nothing!
  }
} // rm

bool Filer::exists(const char *name){
  if(!name || !*name) {
    return false; //pathological
  }
  if(*name != '/') {
    FileName full(name);
    return exists(full.c_str());
  }
  struct stat st;
  if(0 == ::stat(name, &st)) {
    return true;
  } else {
    return false;
  }
} // exists

void Filer::killfileIfOK(bool ok, const char *pathname){
  if(ok) {
    FileName filname(pathname);
    Glib::RefPtr<Gio::File> moribund(Gio::File::create_for_path(filname));
    moribund->remove();
  }
  delete[] pathname;
}

//#include "sigcuser.h"

static void finishCopy(Glib::RefPtr<Gio::AsyncResult> &result, const sigc::slot<void, bool> whendone){
  Glib::RefPtr<Gio::File> file(Glib::RefPtr<Gio::File>::cast_dynamic(result->get_source_object_base()));
  bool success = false;

  try {
    success = file->copy_finish(result);
  } catch(Glib::Error err) {
    IgnoreGlib(err);
  }
  whendone(success);
} // finishCopy

void Filer::moveFile(const char *from, const char *to){
  if(exists(from)) {
    Glib::RefPtr<Gio::File> destination = Gio::File::create_for_path(to);
    Glib::RefPtr<Gio::File> source = Gio::File::create_for_path(from);
    source->copy_async(destination, sigc::bind(&finishCopy, sigc::bind(&killfileIfOK, strdup(from))), Gio::FILE_COPY_OVERWRITE);
  }
}

int Filer::mv(const char *src, const char *target){
  return rename(src, target); //todo:1 capture errno etc stuff.
}
