#include "filewatcher.h"

BundlerList(FileWatch);

bool FileWatch::handle(const FileEvent& ev)  {
  if (ev.ent.wd == wd) {
    handler(ev);
    return false;
  }
  return true;
}

FileWatcher::FileWatcher(bool blocking) : fd("FileWatcher") {
  if (!fd.preopened(inotify_init1(blocking ? 0 : IN_NONBLOCK))) {
    fd.failure(errno);
  }
}

WatchMarker FileWatcher::addWatch(const char *pathname, uint32_t mask) {
  unsigned wd = BadIndex; //init for debug.
  if (fd.okValue(wd, inotify_add_watch(fd, pathname, mask))) {
    return wd;
  } else {
    return BadIndex;
  }
}

bool FileWatcher::removeWatch(FileEvent &ev) {
  return !fd.failed(inotify_rm_watch(fd, ev.ent.wd)); //maydo: don't call rm_watch if wd is badIndex.
}

bool FileWatcher::hasEvent() {
  return fd.available() >= sizeof(FileEvent);
}

void FileWatcher::nextEvent() {
//manpage sez:  sizeof(struct inotify_event)	+ NAME_MAX + 1  but PATH_MAX seems wiser
//example uses a chunk that can handle at least one but also many events and wanders through it.
/* This is a poor performing but stack minimizing implementation. The poor performance is due to two file reads per event, vs 1 file read for 1 to n events. Until I read the code for reading from a filewatching fd and ensure that it won't deliver partial event structs I am going to stick with this.
*/
  FileEvent ev;
  Indexer stuff(reinterpret_cast<unsigned char *>(&ev.ent), offsetof(inotify_event,name));//stop short of name field as it is not guaranteed to be present.
  if (fd.read(stuff) && stuff.freespace()==0) { //want just and all of the guaranteed present fields.
    if(fd.available() >= ev.ent.len) {//if the rest of the struct is present
      uint8_t namecopy[ev.ent.len];
      ev.filename.wrap(namecopy, ev.ent.len);
      if (fd.read(stuff)) {
        Bundler<FileWatch>::While<const FileEvent&>(&FileWatch::handle,ev);//had to be very explicit with template arg of While, it could not deduce this
      }
    }


  }
} // FileWatcher::nextEvent
