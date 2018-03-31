#include "filewatcher.h"
#include <sys/inotify.h>
#include <limits.h>

FileWatcher::FileWatcher(bool blocking):
  fd("FileWatcher"){
  if(!fd.preopened(inotify_init1(blocking?0:IN_NONBLOCK))){
    fd.failure(errno);
  }
}

int FileWatcher::addWatch(const char *pathname, uint32_t mask){
  int wd=BadIndex;//value given for debug.
  if(fd.okValue(wd,inotify_add_watch( fd, pathname, mask))){
    return wd;
  } else {
    return BadIndex;
  }
}

bool FileWatcher::removeWatch(FileEvent &fe){
  return ! fd.failed(inotify_rm_watch(fd,fe.wd));
}

bool FileWatcher::hasEvent(){
  return fd.available()>= sizeof (FileEvent);
}

void FileWatcher::nextEvent(FileEventHandler *handler){
  u8 buffer[sizeof (FileEvent)+NAME_MAX]  __attribute__ ((aligned(__alignof__(struct FileEvent))));
  Indexer<u8> stuff(buffer,sizeof(buffer));
  if(fd.read(stuff)&& fd.lastRead>=sizeof (FileEvent)){//#sign mismatch OK
    FileEvent &fe(*reinterpret_cast<FileEvent*>(buffer));
    if(stuff.used()>sizeof (FileEvent)+fe.len){
      //now we know fe is a sane FileEvent
      if(handler){
        (*handler)(fe);
      }
    }
  }
}

Indexer<char> FileEvent::name(){
  return Indexer<char>(&namestartshere,len);
}


